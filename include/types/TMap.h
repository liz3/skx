//
// Created by Yann Holme Nielsen on 11/08/2020.
//

#ifndef SKX_TMAP_H
#define SKX_TMAP_H

#include "../Variable.h"
#include <vector>
#include <map>


namespace skx {
struct MapEntry {
  std::string key;
  VariableValue* value;
};
class TMap : public VariableValue {
 public:
  std::vector<MapEntry> value;
  std::map<std::string, VariableValue*> keyMap;
  TMap();
  TMap(std::vector<MapEntry> value);

  virtual ~TMap();
  std::string getStringValue() override;
  VariableValue * copyValue() override;
};
}


#endif //SKX_TMAP_H
