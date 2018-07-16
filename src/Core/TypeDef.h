/* 
 * File:   TypeDef.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:17 AM
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>

class Person;
class PersonIndex;
class Event;
class Reporter;
class Drug;
class IStrategy;
class Therapy;

typedef unsigned long ul;

typedef std::vector<double> DoubleVector;
typedef std::vector<DoubleVector> DoubleVector2;
typedef std::vector<DoubleVector2> DoubleVector3;
typedef std::vector<int> IntVector;
typedef std::vector<int>* IntVectorPtr;
typedef std::vector<IntVector> IntVector2;
typedef std::vector<IntVector2> IntVector3;
typedef std::vector<IntVector*> IntVectorPtrVector;
typedef std::vector<IntVector>* IntVector2Ptr;
typedef std::vector<unsigned int> UIntVector;

typedef std::vector<ul> LongVector;
typedef std::vector<LongVector> LongVector2;

typedef std::vector<std::string> StringVector;
typedef std::vector<StringVector> StringVector2;

typedef std::map<int, int> IntIntMap;

typedef std::vector<Person*> PersonPtrVector;
typedef PersonPtrVector::iterator PersonPtrVectorIterator;

typedef std::vector<PersonPtrVector> PersonPtrVector2;
typedef std::vector<PersonPtrVector2> PersonPtrVector3;
typedef std::vector<PersonPtrVector3> PersonPtrVector4;


typedef std::vector<Event*> EventPtrVector;
typedef std::vector<EventPtrVector> EventPtrVector2;

typedef std::vector<Reporter*> ReporterPtrVector;

typedef std::list<PersonIndex*> PersonIndexPtrList;

typedef std::map<int, Drug*> DrugPtrMap;

typedef std::vector<Therapy*> TherapyPtrVector;
typedef std::vector<IStrategy*> StrategyPtrVector;


struct SeasonalInfo {
  bool enable{false};
  DoubleVector A;
  DoubleVector B;
  DoubleVector C;
  DoubleVector phi;
  DoubleVector min_value;
  friend std::ostream& operator<<(std::ostream& os, const SeasonalInfo& seasonal_info);
};

inline std::ostream& operator<<(std::ostream& os, const SeasonalInfo& seasonal_info) {
  os << "seasonal_info: ";
  return os;
}

struct ImmuneSystemInformation {
  double acquire_rate;
  std::vector<double> acquire_rate_by_age;
  double decay_rate;

  double duration_for_fully_immune;
  double duration_for_naive;

  //    double mean_initial_condition;
  //    double sd_initial_condition;

  double immune_inflation_rate;

  double min_clinical_probability;
  double max_clinical_probability;

  double immune_effect_on_progression_to_clinical;

  double c_min;
  double c_max;

  double alpha_immune;
  double beta_immune;

  double age_mature_immunity;
  double factor_effect_age_mature_immunity;

  ImmuneSystemInformation() : acquire_rate(-1), decay_rate(-1), duration_for_fully_immune(-1), duration_for_naive(-1),
                              immune_inflation_rate(-1), min_clinical_probability(-1), max_clinical_probability(-1), c_min(-1), c_max(-1),
                              alpha_immune(-1), beta_immune(-1) { }
};

struct ParasiteDensityLevel {
  double log_parasite_density_cured;
  double log_parasite_density_from_liver;
  double log_parasite_density_asymptomatic;
  double log_parasite_density_clinical;
  double log_parasite_density_clinical_from;
  double log_parasite_density_clinical_to;
  double log_parasite_density_detectable;
  double log_parasite_density_pyrogenic;

  friend std::ostream& operator<<(std::ostream& os, const ParasiteDensityLevel& pdl) {
    os << "["
      << pdl.log_parasite_density_cured << ","
      << pdl.log_parasite_density_from_liver << ","
      << pdl.log_parasite_density_asymptomatic << ","
      << pdl.log_parasite_density_clinical << ","
      << pdl.log_parasite_density_clinical_from << ","
      << pdl.log_parasite_density_clinical_to << ","
      << pdl.log_parasite_density_detectable << ","
      << pdl.log_parasite_density_pyrogenic
      << "]";
    return os;
  }
};

struct RelativeBittingInformation {
  double max_relative_biting_value;
  int number_of_biting_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma  
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_biting_level_value;
  DoubleVector v_biting_level_density;
};

struct RelativeMovingInformation {
  double max_relative_moving_value;
  int number_of_moving_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma  
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_moving_level_value;
  DoubleVector v_moving_level_density;

  double circulation_percent;
  double length_of_stay_mean;
  double length_of_stay_sd;
  double length_of_stay_theta;
  double length_of_stay_k;

};

struct InitialParasiteInfo {
  int location;
  int parasite_type_id;
  double prevalence;

  InitialParasiteInfo() : location(-1), parasite_type_id(-1), prevalence(-1.0) { };

  InitialParasiteInfo(const int loc, const int p_type, const double pre) : location(loc), parasite_type_id(p_type), prevalence(pre) { };

  //    InitialParasiteInfo(const InitialParasiteInfo& orig){
  //        location = orig.location;
  //        parasite_type_id = orig.parasite_type_id;
  //        prevalence = orig.prevalence;
  //    }

};

struct ImportationParasiteInfo {
  int location;
  int time;
  int parasite_type_id;
  int number;

  ImportationParasiteInfo() : location(-1), time(0), parasite_type_id(-1), number(0) { };

  ImportationParasiteInfo(const int loc, const int p_type, const int dur, const int num) : location(loc), time(dur),
                                                                                           parasite_type_id(p_type), number(num) { };

  //    InitialParasiteInfo(const InitialParasiteInfo& orig){
  //        location = orig.location;
  //        parasite_type_id = orig.parasite_type_id;
  //        prevalence = orig.prevalence;
  //    } 
};

struct ImportationParasitePeriodicallyInfo {
	int location{-1};
  int duration{0};
  int parasite_type_id{-1};
  int number{0};
  int start_day{0};
  
  //    InitialParasiteInfo(const InitialParasiteInfo& orig){
  //        location = orig.location;
  //        parasite_type_id = orig.parasite_type_id;
  //        prevalence = orig.prevalence;
  //    } 
};

struct RelativeInfectivity {
  double sigma;
  double ro_star;

  friend std::ostream& operator<<(std::ostream& os, const RelativeInfectivity& e) {
    os << "[" << e.sigma << "," << e.ro_star << "]";
    return os;
  }
};

struct Allele {
  int value; //we can do char later or map from char to int
  std::string name;
  std::string short_name;
  IntVector mutation_values;
  int mutation_level;
  double daily_cost_of_resistance;

  friend std::ostream& operator<<(std::ostream& os, const Allele& e) {
    os << e.short_name;
    return os;
  }
};

struct Locus {
  std::vector<Allele> alleles;
  int position{};

  friend std::ostream& operator<<(std::ostream& os, const Locus& l) {
    for (const auto& allele : l.alleles) {
      os << allele;
    }
    return os;
  }
};

struct GenotypeInfo {
  std::vector<Locus> loci_vector;

  friend std::ostream& operator<<(std::ostream& os, const GenotypeInfo& genotype_info) {
    for (const auto& loci : genotype_info.loci_vector) {
      os << loci;
    }
    return os;
  }
};

#endif /* TYPEDEF_H */