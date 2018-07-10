/* 
 * File:   Reporter.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:05 PM
 */

#include "Reporter.h"
#include "ConsoleReporter.h"
#include "GuiReporter.h"
#include "AMUReporter.h"
#include "FarmReporter.h"
#include "MultipleLocationGuiReporter.h"
#include "YearlyReporterV1.h"
#include "Model.h"
#include "Core/Random.h"
#include "MonthlyReporter.h"
#include "MonthlyReporterConsole.h"
#include "BurninMonthlyReporter.h"
#include "BurninFarmReporter.h"
#include "BFMonthlyReporter.h"
#include "BFFarmReporter.h"
#include <string>

Reporter::Reporter() : model_(nullptr) {
}

Reporter::~Reporter() {
}

Reporter *Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
    case CONSOLE:
      return new ConsoleReporter();
    case GUI:
      return new GuiReporter();
    case AMU:
      return new AMUReporter();
    case FARM:
      return new FarmReporter();
    case MULTIPLE_LOCATION:
      return new MultipleLocationGuiReporter();
    case YEARLY_REPORTER_V1:
      return new YearlyReporterV1("yearly_output.txt");
    case MONTHLY_REPORTER:
      return new MonthlyReporterConsole();
    case BURNIN_MONTHLY_REPORTER:
      return new BurninMonthlyReporter();
    case BURNIN_FARM_REPORTER:
      return new BurninFarmReporter();
    case BFREPORTER:
      return new BFMonthlyReporter();
    case BFFARM_REPORTER:
      return new BFFarmReporter();
    default:
      return new ConsoleReporter();
  }
}