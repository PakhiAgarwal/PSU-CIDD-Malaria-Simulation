/* 
 * File:   FarmReporter.cpp
 * Author: Merlin
 * 
 * Created on August 17, 2013, 4:45 PM
 */
#include <iostream>
#include "fmt/format.h"
#include "FarmReporter.h"
#include "Model.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"
#include "Therapies/SCTherapy.h"
#include "Constants.h"

FarmReporter::FarmReporter() {
}

FarmReporter::~FarmReporter() {
}

void FarmReporter::initialize() {
}

void FarmReporter::before_run() {
  std::cout << Model::RANDOM->seed() << std::endl;
}

void FarmReporter::after_run() {
  //output parameter
  output_parameters();

  //output others indicators

  // NTF
  print_ntf_by_location();

  //TF at year 15
  std::cout << Model::DATA_COLLECTOR->tf_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->single_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->double_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->triple_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->quadruple_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->quintuple_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->art_resistance_frequency_at_15() << "\t";
  std::cout << Model::DATA_COLLECTOR->total_resistance_frequency_at_15() << "\t";

  // EIR - 20x
  print_EIR_by_location();

  print_20x_by_location();

  // EAMU
  print_EAMU();

  // prevalance
  print_prevalence_by_location();

  // fraction of positive that are clinical
  print_fraction_of_positive_that_are_clinical_by_location();

  // mean immune
  print_mean_immune_by_location();

  // mean number of clones per positive Individual
  print_mean_number_of_clones_per_positive_individual();
  // LOI
  print_LOI();

  // mean number of clones per positive Individual by age group
  print_mean_number_of_clones_per_positive_individual_by_age_group();

  // death by age class
  print_death_by_age_group();

  // number of clinical episode by age class
  print_number_of_clinical_episode_by_age_class();

  // prevalance by age class
  print_prevalence_by_age_class();
  // clinical percentage by age class
  print_fraction_of_positive_that_are_clinical_by_location_age_class();

  print_mean_immune_by_location_age_class();

  //print moi distribution
  print_moi_distribution();

  // resistance tracker
  print_resistance_tracker();

  // number of treatment by therapy
  print_treatments_by_therapy();

  // current time
  std::cout << Model::SCHEDULER->current_time() << "\t";
  // utl
  print_total_utl();

  std::cout << Model::DATA_COLLECTOR->popsize_by_location()[0] << "\t";

  //
  //    print_popsize_by_age_class();

  //print phi value by 5-age-group
  print_fraction_of_positive_that_are_clinical_by_location_age_class_by_5();

  print_utl();

  std::cout << std::endl;
}

void FarmReporter::begin_time_step() {
}

void FarmReporter::monthly_report() {

}

void FarmReporter::output_parameters() {
  std::cout << fmt::format("%d\t%d\t", Model::RANDOM->seed(), Model::CONFIG->number_of_locations());

  //intial population size
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::CONFIG->location_db()[location].population_size << "\t";
  }
  //beta
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::CONFIG->location_db()[location].beta << "\t";
  }
  //p_treatment
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::CONFIG->location_db()[location].p_treatment_less_than_5 << "\t";
  }

  //    std::cout << Model::CONFIG->genotype_info().loci_vector[0].cost_of_resistance << "\t";
  std::cout << Model::CONFIG->genotype_info().loci_vector[0].alleles[1].daily_cost_of_resistance << "\t";
  std::cout << Model::CONFIG->immune_system_information().factor_effect_age_mature_immunity << "\t";
  std::cout << Model::CONFIG->immune_system_information().immune_effect_on_progression_to_clinical << "\t";
  std::cout << Model::CONFIG->relative_bitting_info().max_relative_biting_value << "\t";
  std::cout << Model::CONFIG->relative_bitting_info().mean << "\t";
  std::cout << Model::CONFIG->relative_bitting_info().sd << "\t";
  std::cout << Model::CONFIG->drug_db()->begin()->second->k() << "\t";
  std::cout << Model::CONFIG->drug_db()->begin()->second->p_mutation() << "\t";
  std::cout << Model::TREATMENT_STRATEGY->get_type() << "\t";
  std::cout << Model::CONFIG->tf_window_size() << "\t";

  SCTherapy *scTherapy = dynamic_cast<SCTherapy *> (Model::TREATMENT_STRATEGY->get_therapy(nullptr));
  if (scTherapy!=nullptr) {
    std::cout << scTherapy->dosing_day() << "\t";
  } else {
    std::cout << 0 << "\t";
  }
  //fraction of non-art replacement
  //    std::cout << Model::CONFIG->fraction_non_art_replacement() << "\t";

}

void FarmReporter::print_ntf_by_location() {
  double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period())/
      (double) Constants::DAYS_IN_YEAR();
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    double location_discounted_NTF =
        Model::DATA_COLLECTOR->cumulative_discounted_NTF_by_location()[location]*100/
            (double) Model::DATA_COLLECTOR->popsize_by_location()[location];
    double NTF = Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location]*100/
        (double) Model::DATA_COLLECTOR->popsize_by_location()[location];

    location_discounted_NTF /= total_time_in_years;
    NTF /= total_time_in_years;

    std::cout << location_discounted_NTF << "\t";
    std::cout << NTF << "\t";
  }
}

void FarmReporter::print_EIR_by_location() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::DATA_COLLECTOR->EIR_by_location()[location] << "\t";
  }
}

void FarmReporter::print_20x_by_location() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::DATA_COLLECTOR->percentage_bites_on_top_20_by_location()[location]*100 << "\t";
  }
}

void FarmReporter::print_EAMU() {
  std::cout << Model::DATA_COLLECTOR->AMU_per_parasite_pop() << "\t";
  std::cout << Model::DATA_COLLECTOR->AMU_per_person() << "\t";
  std::cout << Model::DATA_COLLECTOR->AMU_for_clinical_caused_parasite() << "\t";
  std::cout << Model::DATA_COLLECTOR->AFU() << "\t";
}

void FarmReporter::print_prevalence_by_location() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    double prevalence = 0.0;
    for (int i = 0; i < 10; i++) {
      prevalence += Model::DATA_COLLECTOR->last_10_blood_slide_prevalence_by_location()[location][i];
    }
    std::cout << prevalence/10.0 << "\t";
  }
}

void FarmReporter::print_fraction_of_positive_that_are_clinical_by_location() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    double f_clinical = 0.0;
    for (int i = 0; i < 10; i++) {
      f_clinical += Model::DATA_COLLECTOR->last_10_fraction_positive_that_are_clinical_by_location()[location][i];
    }
    std::cout << f_clinical/10.0 << "\t";
  }

}

void FarmReporter::print_mean_immune_by_location() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::DATA_COLLECTOR->total_immune_by_location()[location]/
        Model::DATA_COLLECTOR->popsize_by_location()[location] << "\t";
  }
}

void FarmReporter::print_mean_number_of_clones_per_positive_individual() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    unsigned long totalClones = Model::DATA_COLLECTOR->total_parasite_population_by_location()[location];
    int count = Model::DATA_COLLECTOR->number_of_positive_by_location()[location];
    double r = (count==0) ? 0 : (totalClones/(double) count);
    std::cout << r << "\t";
  }

}

void FarmReporter::print_mean_number_of_clones_per_positive_individual_by_age_group() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      unsigned long
          totalClones = Model::DATA_COLLECTOR->total_parasite_population_by_location_age_group()[location][ac];
      int count = Model::DATA_COLLECTOR->number_of_positive_by_location_age_group()[location][ac];
      double r = (count==0) ? 0 : (totalClones/(double) count);
      std::cout << r << "\t";
    }
  }
}

void FarmReporter::print_death_by_age_group() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {

      std::cout << Model::DATA_COLLECTOR->number_of_death_by_location_age_group()[location][ac] << "\t";
    }
  }
}

void FarmReporter::print_number_of_clinical_episode_by_age_class() {
  double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day())/
      (double) Constants::DAYS_IN_YEAR();
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      std::cout << (Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location_age_group()[location][ac]/
          (double) Model::DATA_COLLECTOR->popsize_by_location_age_class()[location][ac])/
          total_time_in_years
                << "\t";
    }
  }
}

void FarmReporter::print_prevalence_by_age_class() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      std::cout << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location_age_group()[location][ac] << "\t";
    }
  }
}

void FarmReporter::print_fraction_of_positive_that_are_clinical_by_location_age_class() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {

      double s = 0;
      for (int i = 0; i < 10; i++) {
        s +=
            Model::DATA_COLLECTOR->last_10_fraction_positive_that_are_clinical_by_location_age_class()[location][ac][i];
      }
      std::cout << s/10 << "\t";

      //            std::cout << Model::STATISTIC->number_of_clinical_by_location_age_group()[location][ac] / (double) Model::STATISTIC->number_of_positive_by_location_age_group()[location][ac] << "\t";
    }
  }
}

void FarmReporter::print_fraction_of_positive_that_are_clinical_by_location_age_class_by_5() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {

      double s = 0;
      for (int i = 0; i < 10; i++) {
        s += Model::DATA_COLLECTOR
            ->last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5()[location][ac][i];
      }
      std::cout << s/10 << "\t";

      //            std::cout << Model::STATISTIC->number_of_clinical_by_location_age_group()[location][ac] / (double) Model::STATISTIC->number_of_positive_by_location_age_group()[location][ac] << "\t";
    }
  }
}

void FarmReporter::print_resistance_tracker() {
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().any_double_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().all_double_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().any_triple_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().all_triple_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().any_quadruple_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().all_quadruple_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().total_tracking_time()[i] << "\t";
  }
  for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().artemisinin_tracking_time()[i] << "\t";
  }
}

void FarmReporter::print_treatments_by_therapy() {
  for (int t_id = 0; t_id < Model::CONFIG->therapy_db().size(); t_id++) {
    int nTreaments = Model::DATA_COLLECTOR->number_of_treatments_with_therapy_ID()[t_id];
    int nSuccess = Model::DATA_COLLECTOR->number_of_treatments_success_with_therapy_ID()[t_id];
    int nFail = Model::DATA_COLLECTOR->number_of_treatments_fail_with_therapy_ID()[t_id];
    double pSuccess = (nTreaments==0) ? 0 : nSuccess*100.0/(nSuccess + nFail);

    std::cout << t_id << "\t" << nFail + nSuccess << "\t" << nSuccess << "\t" << pSuccess << "\t";

  }
}

void FarmReporter::print_total_utl() {
  std::cout << Model::DATA_COLLECTOR->current_utl_duration() << "\t";
}

void FarmReporter::print_utl() {
  for (int i = 0; i < Model::DATA_COLLECTOR->UTL_duration().size(); i++) {
    std::cout << Model::DATA_COLLECTOR->UTL_duration()[i] << "\t";
  }

}

void FarmReporter::print_LOI() {
  std::cout << Model::CONFIG->immune_system_information().duration_for_naive << "\t";
  std::cout << Model::CONFIG->immune_system_information().duration_for_fully_immune << "\t";
}

void FarmReporter::print_moi_distribution() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int i = 0; i < Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location].size(); i++) {
      std::cout << Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location][i] << "\t";
    }
  }

}

void FarmReporter::print_mean_immune_by_location_age_class() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      std::cout << Model::DATA_COLLECTOR->total_immune_by_location_age_class()[location][ac]/
          Model::DATA_COLLECTOR->popsize_by_location_age_class()[location][ac] << "\t";
    }
  }
}
