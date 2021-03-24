


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <sstream>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"
#include "measure.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() {
}

/*
  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param key
    The local authority code of the Area

  @param value
    The Area object that will contain the Measure objects

  @return
    void

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
*/
void Areas::setArea(std::string key, Area area) {

    bool exists = false;
    AreasContainer existingAreas = this->areas;

    exists = (existingAreas.find(key)->first == key);

    if (exists) {

        std::map<std::string, std::string> newLanguages = area.getLanguages();
        std::map<std::string, Measure> newMeasures = area.getMeasures();

        for (auto it = newLanguages.begin(); it != newLanguages.end(); it++) {

            this->areas.find(key)->second.setName(it->first, it->second);
        }

        for (auto it = newMeasures.begin(); it != newMeasures.end(); it++) {

            this->areas.find(key)->second.setMeasure(it->first, it->second);
        }

    } else {

        this->areas.insert({key, area});
    }
}


/*
  Retrieve an Area instance with a given local authority code.

  @param key
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    ...
    Area area2 = areas.getArea("W06000023");
*/
Area &Areas::getArea(std::string key) {

    for (auto it = this->areas.begin(); it != this->areas.end(); it++) {

        if (key == it->first) {

            return it->second;
        }
    }

    throw std::out_of_range("No area found matching " + key);
}

/*
  Retrieve the number of Areas within the container. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
int Areas::size() const noexcept {

    return this->areas.size();
}

/*
  This function specifically parses the compiled areas.csv file of local 
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  For this coursework, you can assume that areas.csv will always have the same
  three columns in the same order.

  Once the data is parsed, you need to create the appropriate Area objects and
  insert them in to a Standard Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("datasets/areas.csv");
    auto is = input.open();

    auto cols = InputFiles::AREAS.COLS;

    auto areasFilter = BethYw::parseAreasArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(std::istream &is, const BethYw::SourceColumnMapping &cols,
                                         const StringFilterSet * const areasFilter) {

    std::string str;

    try {

        std::getline(is, str);

    } catch (std::runtime_error &e) {

        throw std::runtime_error("Error parsing file");
    }

    while (std::getline(is, str)) {

        bool valid = true;
        size_t firstComma = str.find(',');
        size_t secondComma = str.find(',', firstComma + 1);
        const std::string authorityCode = str.substr(0, firstComma);
        std::string eng = str.substr(firstComma + 1, secondComma - firstComma - 1);
        std::string cym = str.substr(secondComma + 1, std::string::npos - secondComma);

        Area area = Area(authorityCode);

        area.setName("eng", eng);
        area.setName("cym", cym);

        if (areasFilter != nullptr && !areasFilter->empty()) {

            for (auto it = eng.begin(); it != eng.end(); it++) {

                *it = std::tolower(*it);
            }

            for (auto it = cym.begin(); it != cym.end(); it++) {

                *it = std::tolower(*it);
            }

            for (auto it = areasFilter->begin(); it != areasFilter->end(); it++) {

                bool foundEng = eng.find(*it) != eng.npos;
                bool foundCym = cym.find(*it) != cym.npos;

                if (!foundEng && !foundCym && authorityCode != *it) {

                    valid = false;

                } else {

                    valid = true;
                    break;
                }
            }
        }

        if (valid) {

            this->setArea(authorityCode, area);
        }
    }
}

/*
  Data from StatsWales is in the JSON format, and contains three
  top-level keys: odata.metadata, value, odata.nextLink. value contains the
  data we need. Rather than been hierarchical, it contains data as a
  continuous list (e.g. as you would find in a table). For each row in value,
  there is a mapping of various column headings and their respective vaues.

  Therefore, you need to go through the items in value (in a loop)
  using a JSON library. To help you, I've selected the nlohmann::json
  library that you must use for your coursework. Read up on how to use it here:
  https://github.com/nlohmann/json

  Example of using this library:
    - Reading/parsing in from a stream is very simply using the >> operator:
        json j;
        stream >> j;

    - Looping through parsed JSON is done with a simple for each loop. Inside
      the loop, you can access each using the array syntax, with the key/
      column name, e.g. data["Localauthority_ItemName_ENG"] gives you the
      local authority name:
        for (auto& el : j["value"].items()) {
           auto &data = el.value();
           std::string localAuthorityCode = data["Localauthority_ItemName_ENG"];
           // do stuff here...
        }

  In this function, you will have to parse the JSON datasets, extracting
  the local authority code, English name (the files only contain the English
  names), and each measure by year.

  If you encounter an Area that does not exist in the Areas container, you
  should create the Area object

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  I've provided the column names for each JSON file that you need to parse
  as std::strings in datasets.h. This mapping should be passed through to the
  cols parameter of this function.

  Note that in the JSON format, years are stored as strings, but we need
  them as ints. When retrieving values from the JSON library, you will
  have to cast them to the right type.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populateFromWelshStatsJSON(std::istream &is, const BethYw::SourceColumnMapping &cols,
                                       const StringFilterSet *const areasFilter,
                                       const StringFilterSet *const measuresFilter,
                                       const YearFilterTuple *const yearsFilter) {

    json j;
    is >> j;

    for (auto& el : j["value"].items()) {

        bool areaFoundWithCode = true;
        bool areaFoundWithEnglish = true;
        bool areaFoundWithWelsh = true;
        bool measureFound = true;
        bool yearFound = true;
        auto &data = el.value();
        std::string measureLabel;
        std::string measureCodename;
        std::string englishName;
        std::string welshName;
        std::string localAuthorityCode;
        std::string measureKey;
        double measureValue;

        for (auto it = cols.begin(); it != cols.end(); it++) {

            switch (it->first) {

                case BethYw::MEASURE_CODE:

                    measureCodename = data[it->second];
                    break;

                case BethYw::AUTH_CODE:

                    localAuthorityCode = data[it->second];
                    break;

                case BethYw::AUTH_NAME_ENG:

                    englishName = data[it->second];
                    break;

                case BethYw::AUTH_NAME_CYM:

                    welshName = data[it->second];
                    break;

                case BethYw::MEASURE_NAME:

                    measureLabel = data[it->second];
                    break;

                case BethYw::SINGLE_MEASURE_CODE:

                    measureCodename = it->second;
                    break;

                case BethYw::SINGLE_MEASURE_NAME:

                    measureLabel = it->second;
                    break;

                case BethYw::YEAR:

                    measureKey = data[it->second];
                    break;

                case BethYw::VALUE:

                    if (data[it->second].is_number()) {

                        measureValue = data[it->second];

                    } else {

                        std::string measureValueAsString = data[it->second];
                        measureValue = std::stod(measureValueAsString);
                    }

                    break;
            }
        }

        Area area = Area(localAuthorityCode);
        Measure measure = Measure(measureCodename, measureLabel);

        measure.setValue(stoi(measureKey), measureValue);
        area.setName("eng", englishName);
        area.setMeasure(measureCodename, measure);

        if (areasFilter != nullptr && !areasFilter->empty()) {

            for (auto it = englishName.begin(); it != englishName.end(); it++) {

                *it = std::tolower(*it);
            }

            for (auto it = welshName.begin(); it != welshName.end(); it++) {

                *it = std::tolower(*it);
            }

            areaFoundWithCode = (std::find(areasFilter->begin(), areasFilter->end(),
                                     localAuthorityCode) !=
                         areasFilter->end());

            for (auto it = areasFilter->begin(); it != areasFilter->end(); it++) {

                areaFoundWithEnglish = englishName.find(*it) != englishName.npos;
                areaFoundWithWelsh = welshName.find(*it) != welshName.npos;
            }
        }

        if (measuresFilter != nullptr && !measuresFilter->empty()) {

            measureFound = (std::find(measuresFilter->begin(), measuresFilter->end(),
                                      measure.getCodename()) != measuresFilter->end());
        }

        if (yearsFilter != nullptr) {

            int startYear = std::get<0>(yearsFilter[0]);
            int endYear = std::get<1>(yearsFilter[0]);

            if (startYear != 0 && endYear != 0) {

                if (stoi(measureKey) < startYear || stoi(measureKey) > endYear) {

                    yearFound = false;
                }
            }
        }

        if ((areaFoundWithWelsh || areaFoundWithCode || areaFoundWithEnglish) && measureFound &&
        yearFound) {

            this->setArea(area.getLocalAuthorityCode(), area);
        }
    }
}

/*
  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  Note that these files do not include the names for areas, instead you 
  have to rely on the names already populated through 
  Areas::populateFromAuthorityCodeCSV();

  The datasets that will be parsed by this function are
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityByYearCSV(std::istream &is, const BethYw::SourceColumnMapping &cols,
                                           const StringFilterSet *const areasFilter,
                                           const StringFilterSet *const measuresFilter,
                                           const YearFilterTuple *const yearsFilter) {

    std::string measureCodename;
    std::string measureLabel;
    std::string str;
    std::map<int, double> years;

    try {

        for (auto it = cols.begin(); it != cols.end(); it++) {

            if (it->first == BethYw::SINGLE_MEASURE_CODE) {

                measureCodename = it->second;

            } else if (it->first == BethYw::SINGLE_MEASURE_NAME) {

                measureLabel = it->second;
            }
        }
    } catch (std::out_of_range &e) {

        throw std::out_of_range("There were not enough columns in cols.");
    }

    try {

        if (std::getline(is, str)) {

            std::vector<int> commas;

            for (unsigned int i = 0; i < str.size(); i++) {

                if (str[i] == ',') {

                    commas.push_back(i);
                }
            }

            for (unsigned int j = 0; j < commas.size(); j++) {

                int year = std::stoi(str.substr(commas[j] + 1,
                                                commas[j + 1] - (commas[j] + 1)));

                years.insert({year, 0});
            }
        }

        while (std::getline(is, str)) {

            std::vector<double> values;
            std::vector<int> commas;
            Measure measure = Measure(measureCodename, measureLabel);

            for (unsigned int i = 0; i < str.size(); i++) {

                if (str[i] == ',') {

                    commas.push_back(i);
                }
            }

            std::string authorityCode = str.substr(0, commas[0]);
            Area area = Area(authorityCode);

            for (unsigned int j = 0; j < commas.size(); j++) {

                double value = std::stod(str.substr(commas[j] + 1,
                                                    commas[j + 1] - (commas[j] + 1)));

                values.push_back(value);
            }

            bool areasValid = true;
            bool measuresValid = true;

            std::map<std::string, std::string> languages = area.getLanguages();
            std::string eng;
            std::string cym;

            for (auto it = languages.begin(); it != languages.end(); it++) {

                if (it->first == "eng") {

                    eng = it->second;

                } else {

                    cym = it->second;
                }
            }

            if (areasFilter != nullptr && !areasFilter->empty()) {

                for (auto it = eng.begin(); it != eng.end(); it++) {

                    *it = std::tolower(*it);
                }

                for (auto it = cym.begin(); it != cym.end(); it++) {

                    *it = std::tolower(*it);
                }

                for (auto it = areasFilter->begin(); it != areasFilter->end(); it++) {

                    bool foundEng = eng.find(*it) != eng.npos;
                    bool foundCym = cym.find(*it) != cym.npos;

                    if (!foundEng && !foundCym && authorityCode != *it) {

                        areasValid = false;

                    } else {

                        areasValid = true;
                        break;
                    }
                }
            }

            for (auto it = measureCodename.begin(); it != measureCodename.end(); it++) {

                *it = std::tolower(*it);
            }

            if (measuresFilter != nullptr && !measuresFilter->empty()) {

                for (auto it = measuresFilter->begin(); it != measuresFilter->end(); it++) {

                    if (measureCodename != *it) {

                        measuresValid = false;

                    } else {

                        measuresValid = true;
                        break;
                    }
                }
            }

            if (yearsFilter != nullptr) {

                int startYear = std::get<0>(yearsFilter[0]);
                int endYear = std::get<1>(yearsFilter[0]);

                if (startYear != 0 && endYear != 0) {

                    int i = 0;

                    for (auto it = years.begin(); it != years.end(); it++) {

                        if (it->first >= startYear && it->first <= endYear) {

                            it->second = values[i];
                            measure.setValue(it->first, it->second);
                        }

                        i++;
                    }
                }
            }

            std::map<int, double> measureYears = measure.getYears();

            for (auto it = measureYears.begin(); it != measureYears.end(); it++) {

                if (it->second == 0) {

                    measureYears.erase(it);
                }
            }

            if (areasValid && measuresValid) {

                area.setMeasure(measureCodename, measure);
                this->setArea(authorityCode, area);
            }
        }
    } catch (std::runtime_error &e) {

        throw std::runtime_error("There was an error parsing the file.");
    }
}

/*
  Parse data from an standard input stream `is`, that has data of a particular
  `type`, and with a given column mapping in `cols`.

  This function should look at the `type` and hand off to one of the three 
  functions populate………() functions.

  The function must check if the stream is in working order and has content.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols);
*/
void Areas::populate(std::istream &is, const BethYw::SourceDataType &type,
                     const BethYw::SourceColumnMapping &cols) {

    if (type == BethYw::AuthorityCodeCSV) {

        try {

            populateFromAuthorityCodeCSV(is, cols);

        } catch (std::out_of_range &e) {

            throw std::out_of_range("Not enough columns in cols");
        }

    } else if (type == BethYw::AuthorityByYearCSV) {

        populateFromAuthorityByYearCSV(is, cols);

    } else if (type == BethYw::WelshStatsJSON) {

        populateFromWelshStatsJSON(is, cols);

    } else if (type == BethYw::None) {

        throw std::runtime_error("Areas::populate: Unexpected data type");

    }
}

/*
  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This function should look at the `type` and hand off to one of the three 
  functions you've implemented above.

  The function must check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variables areasFilter, measuresFilter,
    and yearsFilter are created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(std::istream &is, const BethYw::SourceDataType &type,
                     const BethYw::SourceColumnMapping &cols,
                     const StringFilterSet * const areasFilter,
                     const StringFilterSet * const measuresFilter,
                     const YearFilterTuple * const yearsFilter) {

    if (type == BethYw::AuthorityCodeCSV) {

        try {

            populateFromAuthorityCodeCSV(is, cols, areasFilter);

        } catch (std::out_of_range &e) {

            throw std::out_of_range("Not enough columns in cols");
        }

    } else if (type == BethYw::AuthorityByYearCSV) {

        populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter, yearsFilter);

    } else if (type == BethYw::WelshStatsJSON) {

        populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);

    } else if (type == BethYw::None) {

        throw std::runtime_error("Areas::populate: Unexpected data type");
    }
}

/*
  Convert this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.

  Use the sample JSON library as above to create this. Construct a blank
  JSON object:
    json j;

  Convert this json object to a string:
    j.dump();

  You then need to loop through your areas, measures, and years/values
  adding this data to the JSON object.

  Read the documentation for how to convert your outcome code to JSON:
    https://github.com/nlohmann/json#arbitrary-types-conversions
  
  The JSON should be formatted as such:
    {
    "<localAuthorityCode1>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               },
    "<localAuthorityCode2>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    ...
    "<localAuthorityCodeN>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    }

  An empty JSON is "{}" (without the quotes), which you must return if your
  Areas object is empty.
  
  @return
    std::string of JSON

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {

  json j;
  json names;
  json years;
  json measuresjson;
  json areasjson;

  if (this->areas.empty()) {

      j = json({});

  } else {

      for (auto it = this->areas.begin(); it != this->areas.end(); it++) {

          auto languages = it->second.getLanguages();
          auto measures = it->second.getMeasures();

          for (auto it1 = languages.begin(); it1 != languages.end(); it1++) {

              names[it1->first] = it1->second;
          }

          for (auto it2 = measures.begin(); it2 != measures.end(); it2++) {

              auto currentYears = it2->second.getYears();

              for (auto it3 = currentYears.begin(); it3 != currentYears.end(); it3++) {

                  years[std::to_string(it3->first)] = it3->second;
              }

              measuresjson[it2->first] = years;
          }

          j[it->first] = {{"measures", measuresjson}, {"names", names}};
      }
  }
  
  return j.dump();
}

/*
  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  See the coursework specification for more information, although for reference
  here is a quick example of how output should be formatted:

    <English name of area 1> / <Welsh name of area 1> (<authority code 1>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>


    <English name of area 2> / <Welsh name of area 2> (<authority code 2>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

    ...

    <English name of area y> / <Welsh name of area y> (<authority code y>)
    <Measure 1 name> (<Measure 1 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x codename>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys Môn (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/
std::ostream &operator<<(std::ostream &os, const Areas &areasObject) {

    for (auto it = areasObject.areas.begin(); it != areasObject.areas.end(); it++) {

        os << it->second << std::endl;
    }

    return os;
}

