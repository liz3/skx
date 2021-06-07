//
// Created by liz3 on 16/08/2020.
//

#ifndef SKX_TPLAYERREF_H
#define SKX_TPLAYERREF_H

#include "../Variable.h"
#include "../Instruction.h"
#include "../api/RuntimeMcEventValues.h"

namespace skx {
class TPlayerRef : public VariableValue {
 public:
  TPlayerRef();
  TriggerEvent* eventRef = nullptr;
  TriggerCommand* command = nullptr;

  TPlayerRef(TriggerEvent *eventRef);

  TPlayerRef(TriggerCommand *command);

  std::string getStringValue() override;

  PlayerName *playerName = nullptr;
};
}


#endif //SKX_TPLAYERREF_H
