


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  This file contains the implementation for the Area class. Area is a relatively
  simple class that contains a local authority code, a container of names in
  different languages (perhaps stored in an associative container?) and a series
  of Measure objects (also in some form of container).

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <sstream>

#include "area.h"

/*
  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area

  @example
    Area("W06000023");
*/
Area::Area(const std::string localAuthorityCode): local_authority_code(localAuthorityCode) {
}

/*
  Retrieve the local authority code for this Area. This function should be 
  callable from a constant context and not modify the state of the instance.
  
  @return
    The Area's local authority code

  @example
    Area area("W06000023");
    ...
    auto authCode = area.getLocalAuthorityCode();
*/
std::string Area::getLocalAuthorityCode() const {

    return local_authority_code;
}

/*
  Get a name for the Area in a specific language.  This function should be 
  callable from a constant context and not modify the state of the instance.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);
    ...
    auto name = area.getName(langCode);
*/
std::string Area::getName(std::string lang) const {

    bool exists = false;
    std::map<std::string, std::string> existingLanguages = this->getLanguages();

    exists = (existingLanguages.find(lang)->first == lang);

    if (exists) {

        return existingLanguages.find(lang)->second;
    }

    throw std::out_of_range("lang does not correspond to a language of a name stored.");
}

/*
  Set a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code

  @example
    Area area("W06000023");
    std::string langCodeEnglish  = "eng";
    std::string langValueEnglish = "Powys";
    area.setName(langCodeEnglish, langValueEnglish);

    std::string langCodeWelsh  = "cym";
    std::string langValueWelsh = "Powys";
    area.setName(langCodeWelsh, langValueWelsh);
*/
void Area::setName(std::string lang, std::string name) {

    if (lang.size() != 3) {

        throw std::invalid_argument("Area::setName: Language code must be three alphabetical "
                                    "letters only");
    }

    for (auto it = lang.begin(); it != lang.end(); it++) {

        if (!isalpha(*it)) {

            throw std::invalid_argument("Area::setName: Language code must be three alphabetical "
                                        "letters only");
        }
    }

    for (auto it = lang.begin(); it != lang.end(); it++) {

        *it = std::tolower(*it);
    }

    std::map<std::string, std::string> existingLanguages = this->getLanguages();
    bool exists = false;

    exists = (existingLanguages.find(lang)->first == lang);

    if (exists) {

        this->languages.find(lang)->second = name;

    } else {

        this->languages.insert({lang, name});
    }
}

/*
  Retrieve a Measure object, given its codename. This function should be case
  insensitive when searching for a measure.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>

  @example
    Area area("W06000023");
    Measure measure("Pop", "Population");
    area.setMeasure("Pop", measure);
    ...
    auto measure2 = area.getMeasure("pop");
*/
Measure &Area::getMeasure(const std::string key) {

    bool found = false;
    std::map<std::string, Measure> measure = this->getMeasures();

    found = (measure.find(key)->first == key);

    if (found) {

        return this->measures.find(key)->second;
    }

    throw std::out_of_range("No measure found matching " + key);
}

/*
  Add a particular Measure to this Area object. Note that the Measure's
  codename should be converted to lowercase.

  If a Measure already exists with the same codename in this Area, overwrite any
  values contained within the existing Measure with those in the new Measure
  passed into this function. The resulting Measure stored inside the Area
  instance should be a combination of the two Measures instances.

  @param key
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void

  @example
    Area area("W06000023");

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    double value = 12345678.9;
    measure.setValue(1999, value);

    area.setMeasure(code, measure);
*/
void Area::setMeasure(std::string key, const Measure measure) {

    for (auto it = key.begin(); it != key.end(); it++) {

        *it = std::tolower(*it);
    }

    bool exists;
    std::map<std::string, Measure> existingMeasures = this->getMeasures();

    exists = (existingMeasures.find(key)->first == key);

    if (exists) {

        std::map<int, double> existingYears = this->measures.find(key)->second.getYears();
        std::map<int, double> newYears = measure.getYears();

        for (auto it = newYears.begin(); it != newYears.end(); it++) {

            this->measures.find(key)->second.setValue(it->first, it->second);
        }

    } else {

        this->measures.insert({key, measure});
    }
}

/*
  Retrieve the number of Measures we have for this Area. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The size of the Area (i.e., the number of Measures)

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    area.setMeasure(code, measure);
    auto size = area.size();
*/
int Area::size() const noexcept {

    return this->measures.size();
}


/*
  Overload the stream output operator as a free/global function.

  Output the name of the Area in English and Welsh, followed by the local
  authority code. Then output all the measures for the area (see the coursework
  worksheet for specific formatting).

  If the Area only has only one name, output this. If the area has no names,
  output the name "Unnamed".

  Measures should be ordered by their Measure codename. If there are no measures
  output the line "<no measures>" after you have output the area names.

  See the coursework specification for more examples.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream

  @example
    Area area("W06000023");
    area.setName("eng", "Powys");
    std::cout << area << std::endl;
*/
std::ostream &operator<<(std::ostream& os, const Area& area) {

    std::stringstream name;
    std::stringstream measure;
    std::map<std::string, std::string> languages = area.getLanguages();

    if (languages.size() > 1) {

        for (auto it = ++languages.begin(); it != languages.end(); it++) {

            name << it->second << " / ";
        }

    }

    name << languages.begin()->second;

    name << " (" << area.getLocalAuthorityCode() << ")" << std::endl;

    if (area.measures.empty()) {

        measure << "<no measures>";

    } else {

        for (auto it = area.measures.begin(); it != area.measures.end(); it++) {

            measure << it->second;
        }
    }

    os << name.str() << measure.str() << std::endl;

    return os;
}

/*
  Overload the == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param lhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.

  @example
    Area area1("MYCODE1");
    Area area2("MYCODE1");

    bool eq = area1 == area2;
*/
bool operator==(const Area& lhs, const Area& rhs) {

    return (lhs.getLocalAuthorityCode() == rhs.getLocalAuthorityCode()) &&
    (lhs.getLanguages() == rhs.getLanguages()) && (lhs.getMeasures() == rhs.getMeasures());
}

std::map<std::string, std::string> Area::getLanguages() const {

    return this->languages;
}

std::map<std::string, Measure> Area::getMeasures() const {

    return this->measures;
}
