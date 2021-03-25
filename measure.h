#ifndef MEASURE_H_
#define MEASURE_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  This file contains the decalaration of the Measure class.
 */

#include <string>
#include <map>

/*
  The Measure class contains a measure code, label, and a container for readings
  from across a number of years.
*/
class Measure {

private:
    std::string codename;
    std::string label;
    std::map<int, double> years;

public:
    Measure(std::string code, const std::string &label);
    std::string getCodename() const noexcept;
    std::string getLabel() const noexcept;
    void setLabel(std::string newLabel);
    double getValue(int key);
    void setValue(int key, double value);
    int size();
    double getDifference() const noexcept;
    double getDifferenceAsPercentage() const noexcept;
    double getAverage() const noexcept;
    friend std::ostream &operator<<(std::ostream &os, const Measure &measure);
    friend bool operator==(const Measure& lhs, const Measure& rhs);
    std::map<int, double> getYears() const noexcept;
};

#endif // MEASURE_H_