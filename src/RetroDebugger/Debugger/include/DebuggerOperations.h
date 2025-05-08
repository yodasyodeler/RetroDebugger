#pragma once

#include "DebuggerCommon.h"

class DebuggerOperations {
public:
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

    std::vector<ArgumentPtr> m_argumentList;
    std::vector<RegisterInfoPtr> m_registerList;
    std::vector<OperationInfoPtr> m_operationList;
};