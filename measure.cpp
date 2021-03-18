


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data. The data you need to 
  store is values, organised by year. I'd recommend storing the values as 
  doubles.
*/

#include <stdexcept>
#include <string>

#include "measure.h"

/*
  Construct a single Measure, that has values across many years.

  All StatsWales JSON files have a codename for measures. You should convert 
  all codenames to lowercase.

  @param codename
    The codename for the measure

  @param label
    Human-readable (i.e. nice/explanatory) label for the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);
*/
Measure::Measure(std::string codename, const std::string &label) {

    for (auto it = codename.begin(); it != codename.end(); it++) {

        *it = std::tolower(*it);
    }

    this->codename = codename;
    this->label = label;
}

/*
  Retrieve the code for the Measure. This function should be callable from a 
  constant context and must promise to not modify the state of the instance or 
  throw an exception.

  @return
    The codename for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto codename2 = measure.getCodename();
*/
std::string Measure::getCodename() const noexcept {

    return this->codename;
}

/*
  Retrieve the human-friendly label for the Measure. This function should be 
  callable from a constant context and must promise to not modify the state of 
  the instance and to not throw an exception.

  @return
    The human-friendly label for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto label = measure.getLabel();
*/
std::string Measure::getLabel() const noexcept {

    return this->label;
}

/*
  Change the label for the Measure.

  @param label
    The new label for the Measure

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    ...
    measure.setLabel("New Population");
*/
void Measure::setLabel(std::string label) {

    this->label = label;
}

/*
  Retrieve a Measure's value for a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto value = measure.getValue(1999); // returns 12345678.9
*/
double Measure::getValue(int key) {

    for (auto it = years.begin(); it != years.end(); it++) {

        if (it->first == key) {

            return it->second;
        }
    }

    throw std::out_of_range("No value found for year " + std::to_string(key));
}

/*
  Add a particular year's value to the Measure object. If a value already
  exists for the year, replace it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year

  @return
    void

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
*/
void Measure::setValue(int key, double value) {

    for (auto it = this->years.begin(); it != this->years.end(); it++) {

        if (it->first == key) {

            it->second = value;
        }
    }

    this->years.insert({key, value});
}

/*
  Retrieve the number of years data we have for this measure. This function
  should be callable from a constant context and must promise to not change
  the state of the instance or throw an exception.

  @return
    The size of the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    auto size = measure.size(); // returns 1
*/
int Measure::size() {

    return years.size();
}

/*
  Calculate the difference between the first and last year imported. This
  function should be callable from a constant context and must promise to not
  change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(1999, 12345679.9);
    auto diff = measure.getDifference(); // returns 1.0
*/
double Measure::getDifference() const noexcept {

    double firstYear = this->years.begin()->second;
    double lastYear = this->years.end()->second;
    double difference = 0;

    if (firstYear != lastYear) {

        difference = lastYear - firstYear;
    }

    return difference;
}

/*
  Calculate the difference between the first and last year imported as a 
  percentage. This function should be callable from a constant context and
  must promise to not change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1990, 12345678.9);
    measure.setValue(2010, 12345679.9);
    auto diff = measure.getDifferenceAsPercentage();
*/
double Measure::getDifferenceAsPercentage() const noexcept {

    double firstYear = this->years.begin()->second;
    double difference = getDifference();
    double percentage = 0;

    if (difference != 0) {

        percentage = (getDifference() / firstYear) * 100;
    }

    return percentage;
}

/*
  Calculate the average/mean value for all the values. This function should be
  callable from a constant context and must promise to not change the state of 
  the instance or throw an exception.

  @return
    The average value for all the years, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(1999, 12345679.9);
    auto diff = measure.getDifference(); // returns 1
*/
double Measure::getAverage() const noexcept {

    double total = 0;
    double average = 0;

    for (auto it = this->years.begin(); it != this->years.end(); it++) {

        total += it->second;
    }

    average = total / this->years.size();

    return average;
}

/*
  Overload the << operator to print all of the Measure's imported data.

  We align the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, correspodning to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, print the name and code, and 
  on the next line print: <no data>

  See the coursework specification for more information.

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    std::cout << measure << std::end;
*/
std::ostream &operator<<(std::ostream &os, const Measure &measure) {

    std::string firstRow = measure.getLabel() + " (" + measure.getCodename() + ")\n";
    std::string secondRow;
    std::string thirdRow = "\n";

    for (auto it = measure.years.begin(); it != measure.years.end(); it++) {

        secondRow += "\t" + std::to_string(it->first);
        thirdRow += " " + std::to_string(it->second);
    }

    secondRow += "\tAverage\tDiff.\t% Diff.";
    thirdRow += " " + std::to_string(measure.getAverage()) + " " +
            std::to_string(measure.getDifference()) + " " +
            std::to_string(measure.getDifferenceAsPercentage());

    return os << firstRow << secondRow << thirdRow;
}

/*
  Overload the == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param lhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/
bool operator==(const Measure& lhs, const Measure& rhs) {

    return ((lhs.getCodename() == rhs.getCodename()) && (lhs.getLabel() == rhs.getLabel()) &&
    (lhs.years == rhs.years));
}

std::map<int, double> Measure::getYears() const {

    return this->years;
}