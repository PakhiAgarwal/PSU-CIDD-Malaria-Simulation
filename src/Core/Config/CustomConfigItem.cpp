#define _USE_MATH_DEFINES

#include "CustomConfigItem.h"
#include "Config.h"
#include "Spatial/SpatialModelBuilder.h"
#include "Helpers/ObjectHelpers.h"
#include "Helpers/NumberHelpers.h"
#include <gsl/gsl_cdf.h>
#include <cmath>
#include <date/date.h>

void total_time::set_value(const YAML::Node& node) {
  value_ = (date::sys_days{config_->ending_date()} - date::sys_days(config_->starting_date())).count();
}

void number_of_age_classes::set_value(const YAML::Node& node) {
  value_ = config_->age_structure().size();
}

void number_of_locations::set_value(const YAML::Node& node) {
  value_ = config_->location_db().size();
}

void spatial_distance_matrix::set_value(const YAML::Node& node) {
  value_.resize(config_->number_of_locations());
  for (auto from_location = 0; from_location < config_->number_of_locations(); from_location++) {
    value_[from_location].resize(static_cast<unsigned long long int>(config_->number_of_locations()));
    for (auto to_location = 0; to_location < config_->number_of_locations(); to_location++) {
      value_[from_location][to_location] = Spatial::Coordinate::calculate_distance_in_km(
        *config_->location_db()[from_location].coordinate,
        *config_->location_db()[to_location].coordinate);
      //
      //            std::cout << "distance[" << from_location << "," << to_location << "]: "
      //                      << spatial_distance_matrix_[from_location][to_location] << std::endl;
    }
  }

}

void seasonal_info::set_value(const YAML::Node& node) {
  auto seasonal_info_node = node[name_];
  value_.A.clear();
  value_.B.clear();
  value_.C.clear();
  value_.phi.clear();
  value_.min_value.clear();
  value_.enable = seasonal_info_node["enable"].as<bool>();

  for (auto i = 0; i < config_->number_of_locations(); i++) {
    auto input_loc = seasonal_info_node["a"].size() < config_->number_of_locations() ? 0 : i;
    value_.A.push_back(seasonal_info_node["a"][input_loc].as<double>());

    const auto period = seasonal_info_node["period"].as<double>();
    auto B = 2 * M_PI / period;

    value_.B.push_back(B);

    const auto phi = seasonal_info_node["phi"][input_loc].as<float>();
    value_.phi.push_back(phi);
    auto C = -phi * B;
    value_.C.push_back(C);

    value_.min_value.push_back(seasonal_info_node["min_value"][input_loc].as<float>());
  }
}


spatial_model::~spatial_model() {
  if (value_ != nullptr) {
    delete value_;
    value_ = nullptr;
  }
}

void spatial_model::set_value(const YAML::Node& node) {
  //read spatial_model
  const auto sm_name = node[name_]["name"].as<std::string>();
  // std::cout << sm_name << std::endl;
  value_ = Spatial::SpatialModelBuilder::Build(sm_name, node[name_][sm_name]);
}

void immune_system_information::set_value(const YAML::Node& node) {
  auto is_node = node[name_];
  value_.acquire_rate = is_node["b1"].as<double>();
  value_.decay_rate = is_node["b2"].as<double>();

  value_.duration_for_fully_immune = is_node["duration_for_fully_immune"].as<double>();
  value_.duration_for_naive = is_node["duration_for_naive"].as<double>();

  const auto mean_initial_condition = is_node["mean_initial_condition"].as<double>();
  const auto sd_initial_condition = is_node["sd_initial_condition"].as<double>();

  if (NumberHelpers::is_equal(sd_initial_condition, 0.0)) {
    value_.alpha_immune = mean_initial_condition;
    value_.beta_immune = 0.0;
  }
  else {
    value_.alpha_immune =
      mean_initial_condition * mean_initial_condition * (1 - mean_initial_condition) /
      (sd_initial_condition * sd_initial_condition) - mean_initial_condition;
    value_.beta_immune = value_.alpha_immune / mean_initial_condition -
      value_.alpha_immune;
  }

  value_.immune_inflation_rate = is_node["immune_inflation_rate"].as<double>();

  value_.min_clinical_probability = is_node["min_clinical_probability"].as<double>();
  value_.max_clinical_probability = is_node["max_clinical_probability"].as<double>();

  value_.immune_effect_on_progression_to_clinical = is_node[
    "immune_effect_on_progression_to_clinical"].as<double>();

  //    std::cout << value_.c_min << std::endl;
  //    std::cout << value_.c_max << std::endl;


  value_.age_mature_immunity = is_node["age_mature_immunity"].as<double>();
  value_.factor_effect_age_mature_immunity = is_node["factor_effect_age_mature_immunity"].as<double>();

  // implement inlation rate
  double acR = value_.acquire_rate;
  value_.acquire_rate_by_age.clear();
  for (int age = 0; age <= 80; age++) {
    double factor = 1;
    if (age < value_.age_mature_immunity) {
      factor = (age == 0) ? 0.5 : age;

      factor = factor / value_.age_mature_immunity;
      factor = pow(factor, value_.factor_effect_age_mature_immunity);
    }

    value_.acquire_rate_by_age.push_back(factor * acR);

    acR *= (1 + value_.immune_inflation_rate);
    //        std::cout << acR << std::endl;
  }
  assert(value_.acquire_rate_by_age.size() == 81);

  value_.c_min = pow(10, -(config_->parasite_density_level().log_parasite_density_asymptomatic -
                       config_->parasite_density_level().log_parasite_density_cured) /
                     value_.duration_for_fully_immune);
  value_.c_max = pow(10, -(config_->parasite_density_level().log_parasite_density_asymptomatic -
                       config_->parasite_density_level().log_parasite_density_cured) /
                     value_.duration_for_naive);
}

genotype_db::~genotype_db() {
  ObjectHelpers::delete_pointer<IntGenotypeDatabase>(value_);
}

void genotype_db::set_value(const YAML::Node& node) {

  value_ = new IntGenotypeDatabase();

  value_->weight().clear();
  value_->weight().assign(config_->genotype_info().loci_vector.size(), 1);

  auto temp = 1;
  for (int i = value_->weight().size() - 2; i > -1; i--) {
    temp *= config_->genotype_info().loci_vector[i + 1].alleles.size();
    value_->weight()[i] = temp;
  }
  auto number_of_genotypes = 1;
  // std::cout << config_->genotype_info().loci_vector.size() << std::endl;
  for (auto& locus : config_->genotype_info().loci_vector) {
    number_of_genotypes *= locus.alleles.size();
  }

  for (auto i = 0; i < number_of_genotypes; i++) {
    auto* int_genotype = new IntGenotype(i, config_->genotype_info(), value_->weight());
    //        std::cout << *int_genotype << std::endl;
    value_->add(int_genotype);
  }
  
  value_->initialize_matting_matrix();
}

void number_of_parasite_types::set_value(const YAML::Node& node) {
  value_ = static_cast<int>(config_->genotype_db()->db().size());
}

drug_db::~drug_db() {
  ObjectHelpers::delete_pointer<DrugDatabase>(value_);
}

void drug_db::set_value(const YAML::Node& node) {
  ObjectHelpers::delete_pointer<DrugDatabase>(value_);
  value_ = new DrugDatabase();

  for (auto drug_id = 0; drug_id < node[name_].size(); drug_id++) {
    auto* dt = new DrugType();
    dt->set_id(drug_id);

    const auto i_s = NumberHelpers::number_to_string<int>(drug_id);
    const auto& dt_node = node[name_][i_s];

    dt->set_drug_half_life(dt_node["half_life"].as<double>());
    dt->set_maximum_parasite_killing_rate(dt_node["maximum_parasite_killing_rate"].as<double>());
    dt->set_n(dt_node["n"].as<double>());
    //    dt->set_EC50(node["EC50"].as<double>());

    //    std::cout <<dt->drug_half_life() << "-" << dt->maximum_parasite_killing_rate() << "-" << dt->n() << "-" << dt->EC50() << std::endl;
    for (auto i = 0; i < dt_node["age_specific_drug_concentration_sd"].size(); i++) {
      dt->age_group_specific_drug_concentration_sd().push_back(
        dt_node["age_specific_drug_concentration_sd"][i].as<double>());
    }
    //    assert(dt->age_group_specific_drug_concentration_sd().size() == 15);

    dt->set_p_mutation(dt_node["mutation_probability"].as<double>());

    dt->affecting_loci().clear();
    for (auto i = 0; i < dt_node["affecting_loci"].size(); i++) {
      dt->affecting_loci().push_back(dt_node["affecting_loci"][i].as<int>());
    }

    dt->selecting_alleles().clear();
    dt->selecting_alleles().assign(dt_node["affecting_loci"].size(), IntVector());
    for (auto i = 0; i < dt_node["affecting_loci"].size(); i++) {
      for (auto j = 0; j < dt_node["selecting_alleles"][i].size(); j++) {
        dt->selecting_alleles()[i].push_back(dt_node["selecting_alleles"][i][j].as<int>());

      }
    }

    dt->set_ec50_map(dt_node["EC50"].as<std::map<std::string, double>>());

    //    auto ec50Node = node["EC50"];
    //    for (YAML::const_iterator it = ec50Node.begin(); it != ec50Node.end(); it++) {
    //        std::string key = it->first.as<std::string>();
    //        double value = it->second.as<double>();
    //        std::cout << key << ":::::" << value << std::endl;
    //    }

    dt->set_k(dt_node["k"].as<double>());

    // auto* dt = read_drugtype(config, i);
    //        std::cout << i << std::endl;
    value_->add(dt);

  }
}

void EC50_power_n_table::set_value(const YAML::Node& node) {
   //get EC50 table and compute EC50^n
   value_.clear();
   value_.assign(config_->genotype_db()->db().size(), std::vector<double>());
  
   for (auto g_id = 0; g_id < config_->genotype_db()->db().size(); g_id++) {
     for (auto i = 0; i < config_->drug_db()->drug_db().size(); i++) {
       value_[g_id].push_back(config_->drug_db()->drug_db()[i]->infer_ec50(config_->genotype_db()->db()[g_id]));
     }
   }
   //    std::cout << "ok " << std::endl;
  
   for (auto g_id = 0; g_id < config_->genotype_db()->db().size(); g_id++) {
     for (auto i = 0; i < config_->drug_db()->drug_db().size(); i++) {
       value_[g_id][i] = pow(value_[g_id][i], config_->drug_db()->get(i)->n());
     }
   }
}

void circulation_info::set_value(const YAML::Node& in_node) {
  auto node = in_node[name_];
  value_.max_relative_moving_value = node["max_relative_moving_value"].as<double>();

  value_.number_of_moving_levels = node["number_of_moving_levels"].as<int>();

  value_.scale = node["moving_level_distribution"]["Exponential"]["scale"].as<double>();

  value_.mean = node["moving_level_distribution"]["Gamma"]["mean"].as<double>();
  value_.sd = node["moving_level_distribution"]["Gamma"]["sd"].as<double>();

  //calculate density and level value here

  const auto var = value_.sd * value_.sd;

  const auto b = var / (value_.mean - 1); //theta
  const auto a = (value_.mean - 1) / b; //k

  value_.v_moving_level_density.clear();
  value_.v_moving_level_value.clear();

  const auto max = value_.max_relative_moving_value - 1; //maxRelativeBiting -1
  const auto number_of_level = value_.number_of_moving_levels;

  const auto step = max / static_cast<double>(number_of_level - 1);

  auto j = 0;
  double old_p = 0;
  double sum = 0;

  for (double i = 0; i <= max + 0.0001; i += step) {
    const auto p = gsl_cdf_gamma_P(i + step, a, b);
    double value = 0;
    value = (j == 0) ? p : p - old_p;
    value_.v_moving_level_density.push_back(value);
    old_p = p;
    value_.v_moving_level_value.push_back(i + 1);
    sum += value;
    j++;

  }

  //normalized
  double t = 0;
  for (auto& i : value_.v_moving_level_density) {
    i = i + (1 - sum) / value_.v_moving_level_density.size();
    t += i;
  }

  assert(value_.number_of_moving_levels == value_.v_moving_level_density.size());
  assert(value_.number_of_moving_levels == value_.v_moving_level_value.size());
  assert(fabs(t - 1) < 0.0001);

  value_.circulation_percent = node["circulation_percent"].as<double>();

  const auto length_of_stay_mean = node["length_of_stay"]["mean"].as<double>();
  const auto length_of_stay_sd = node["length_of_stay"]["sd"].as<double>();

  const auto stay_variance = length_of_stay_sd * length_of_stay_sd;
  const auto k = stay_variance / length_of_stay_mean; //k
  const auto theta = length_of_stay_mean / k; //theta

  value_.length_of_stay_theta = theta;
  value_.length_of_stay_k = k;
}

void relative_bitting_info::set_value(const YAML::Node& in_node) {
  auto node = in_node[name_];
  value_.max_relative_biting_value = node["max_relative_biting_value"].as<double>();

  value_.number_of_biting_levels = node["number_of_biting_levels"].as<int>();

  value_.scale = node["biting_level_distribution"]["Exponential"]["scale"].as<double>();

  value_.mean = node["biting_level_distribution"]["Gamma"]["mean"].as<double>();
  value_.sd = node["biting_level_distribution"]["Gamma"]["sd"].as<double>();


  const auto var = value_.sd * value_.sd;
  const auto b = var / (value_.mean - 1); //theta
  const auto a = (value_.mean - 1) / b; //k

  value_.v_biting_level_density.clear();
  value_.v_biting_level_value.clear();

  const auto max = value_.max_relative_biting_value - 1; //maxRelativeBiting -1
  const auto number_of_level = value_.number_of_biting_levels;

  const auto step = max / static_cast<double>(number_of_level - 1);

  auto j = 0;
  auto old_p = 0.0;
  auto sum = 0.0;

  for (double i = 0; i <= max + 0.0001; i += step) {
    const auto p = gsl_cdf_gamma_P(i + step, a, b);
    double value = 0;
    value = (j == 0) ? p : p - old_p;
    value_.v_biting_level_density.push_back(value);
    old_p = p;
    value_.v_biting_level_value.push_back(i + 1);
    sum += value;
    j++;

  }


  //normalized
  double t = 0;
  for (auto& i : value_.v_biting_level_density) {
    i = i + (1 - sum) / value_.v_biting_level_density.size();
    t += i;
  }


  assert(value_.number_of_biting_levels ==value_.v_biting_level_density.size());
  assert(value_.number_of_biting_levels ==value_.v_biting_level_value.size());
  assert(fabs(t - 1) < 0.0001);

}