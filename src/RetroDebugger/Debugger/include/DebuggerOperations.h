#pragma once

#include "DebuggerCallbacks.h"
#include "DebuggerCommon.h"

namespace Rdb {

class DebuggerOperations {
public:
    DebuggerOperations(std::shared_ptr<DebuggerCallback> callbacks);

    void Reset();

    Operations GetOperations() const;
    Operations GetJumpOperations() const;
    std::vector<RegisterInfoPtr> GetRegisters() const;

    size_t GetOperation(size_t address, Operation& operation);
    void SetOperations(const XmlOperationsMap& operations);

private:
    void ConvertOperation(OpcodeToOperation& operationMap, const XmlDebuggerOperation& xmlOperation);

    Operations m_operations = {};
    Operations m_jumpOperations = {};

    std::shared_ptr<DebuggerCallback> m_callbacks;
    std::vector<ArgumentPtr> m_argumentList;
    std::vector<RegisterInfoPtr> m_registerList;
    std::vector<OperationInfoPtr> m_operationList;
};

}