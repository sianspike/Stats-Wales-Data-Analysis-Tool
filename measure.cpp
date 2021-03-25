


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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

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
    this->years = std::map<int, double>();
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
  Change the newLabel for the Measure.

  @param newLabel
    The new newLabel for the Measure

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    ...
    measure.setLabel("New Population");
*/
void Measure::setLabel(std::string newLabel) {

    this->label = std::move(newLabel);
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

    std::map<int, double> existingYears = getYears();
    bool found = false;

    found = (existingYears.find(key)->first == key);

    if (found) {

        return years.find(key)->second;
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

    std::map<int, double> existingYears = this->getYears();
    bool exists;

    exists = (existingYears.find(key)->first == key);

    if (exists) {

        this->years.find(key)->second = value;

    } else {

        this->years.insert({key, value});
    }
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
  REFERENCES:
  https://stackoverflow.com/questions/289715/last-key-in-a-stdmap

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

    int firstYear = this->years.begin()->first;
    int lastYear = firstYear;
    double firstYearValue = this->years.begin()->second;
    double lastYearValue = firstYearValue;
    double difference = 0;

    if (this->years.size() > 1) {

        lastYear = (--this->years.end())->first;
        lastYearValue = (--this->years.end())->second;
    }

    if (firstYear != lastYear) {

        difference = lastYearValue - firstYearValue;
    }

    return difference;
}

/*
  REFERENCES:
  https://stackoverflow.com/questions/12764009/how-to-cut-decimal-off-without-rounding-in-c
  https://www.daniweb.com/programming/software-development/code/217332/round-double-to-n-decimal-places
  https://stackoverflow.com/questions/3844010/convert-a-double-to-fixed-decimal-point-in-c
  https://stackoverflow.com/questions/1343890/how-do-i-restrict-a-float-value-to-only-two-places
  -after-the-decimal-point-in-c
  https://www.quora.com/How-can-I-round-a-double-number-to-3-decimal-digits-in-C++
  https://www.codegrepper.com/code-examples/typescript/how+to+round+upto+6+digits+in+cpp
  https://en.cppreference.com/w/cpp/numeric/math/round
  https://stackoverflow.com/questions/9849535/how-to-round-a-double-to-n-decimals

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

        percentage = (difference / firstYear) * 100;
    }

    //make 6 decimal places.
    percentage = (percentage * 1000000) / 1000000;

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
    double average;

    for (auto it = this->years.begin(); it != this->years.end(); it++) {

        total += it->second;
    }

    average = total / this->years.size();

    return average;
}

/*
  REFERENCES:
  https://stackoverflow.com/questions/49000762/c-ostream-operator-overloading-text-formatting
  -while-trying-to-skip-over-inter
  https://stackoverflow.com/questions/49332013/adding-a-new-line-after-stdostream-output-without
  -explicitly-calling-it
  https://stackoverflow.com/questions/35399642/c-ostream-overloading-problems
  https://www.codegrepper.com/code-examples/cpp/overloading+ostream+operator+c%2B%2B

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

    std::stringstream first;
    std::stringstream second;
    std::stringstream third;

    first << measure.getLabel() << " (" << measure.getCodename() << ")" <<
          std::endl;

    for (auto it = measure.years.begin(); it != measure.years.end(); it++) {

        second << std::right << std::setw(int(std::to_string(it->second).size())) <<
        std::setfill(' ') << std::to_string(it->first) << ' ';

        third << std::right << std::setw(int(std::to_string(it->second).size())) <<
        std::setfill(' ') << std::to_string(it->second) << ' ';
    }

    second << std::right << std::setw(int(std::to_string(measure.getAverage()).size())) <<
    std::setfill(' ') << "Average" << ' ';

    second << std::right << std::setw(int(std::to_string(measure.getDifference()).size())) <<
    std::setfill(' ') << "Diff." << ' ';

    second << std::right <<
    std::setw(int(std::to_string(measure.getDifferenceAsPercentage()).size())) <<
    std::setfill(' ') << "% Diff." << std::endl;

    third << std::right << std::setw(int(std::to_string(measure.getAverage()).size())) <<
    std::setfill(' ') << std::fixed << std::setprecision(6) << std::to_string(measure.getAverage
    ()) << ' ';

    third << std::right << std::setw(int(std::to_string(measure.getDifference()).size())) <<
    std::setfill(' ') << std::fixed << std::setprecision(6) << std::to_string(measure
    .getDifference()) << ' ';

    third << std::right <<
    std::setw(int(std::to_string(measure.getDifferenceAsPercentage()).size())) <<
    std::setfill(' ') << std::fixed <<
    std::setprecision(6) << std::to_string(measure.getDifferenceAsPercentage()) << std::endl;

    return os << first.str() << second.str() << third.str() << std::endl;
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

std::map<int, double> Measure::getYears() const noexcept {

    return this->years;
}