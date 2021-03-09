#ifndef AREA_H_
#define AREA_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  This file contains the Area class declaration. Area objects contain all the
  Measure objects for a given local area, along with names for that area and a
  unique authority code.

  TODO: Read the block comments with TODO in area.cpp to know which 
  functions and member variables you need to declare in this class.
 */

#include <string>
#include <list>
#include <set>
#include <unordered_set>
#include <map>

#include "measure.h"

/*
  An Area object consists of a unique authority code, a container for names
  for the area in any number of different languages, and a container for the
  Measures objects.

  TODO: Based on your implementation, there may be additional constructors
  or functions you implement here, and perhaps additional operators you may wish
  to overload.
*/
class Area {

private:
    const std::string &local_authority_code;
    std::map<std::string, std::string> languages;
    std::map<std::string, Measure> measures;

public:
  Area(const std::string& localAuthorityCode);
  std::string getLocalAuthorityCode() const;
  std::string getName(std::string lang) const;
  void setName(std::string lang, std::string name);
  Measure getMeasure(std::string key);
  void setMeasure(std::string key, Measure measure);
  int size() const noexcept;
  friend std::ostream &operator<<(std::ostream& os, const Area& area);
  friend bool operator==(const Area& lhs, const Area& rhs);
};

#endif // AREA_H_