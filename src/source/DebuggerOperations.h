#pragma once

#include "DebuggerCommon.h"

class DebuggerXmlParser;

class DebuggerOperations {
public:
    explicit DebuggerOperations(std::shared_ptr<DebuggerXmlParser> debuggerParser);
    DebuggerOperations(std::shared_ptr<DebuggerXmlParser> debuggerParser, const std::string& filename);

    void Reset();

    bool IsValid() const;
    std::string GetErrorMessage();
    Operations GetOperations() const;
    Operations GetJumpOpertions() const;
    std::vector<RegisterInfoPtr> GetRegisters() const;

    size_t GetOperation(size_t address, Operation& operation);
    bool ParseFile(const std::string& filename);

private:
    void ConvertOperation(OpcodeToOperation& operationMap, const XmlDebuggerOperation& xmlOperation);

    bool m_isValid = false;
    Operations m_operations = {};
    Operations m_jumpOperations = {};
    std::shared_ptr<DebuggerXmlParser> m_debuggerParser;
    std::vector<ArgumentPtr> m_argumentList;
    std::vector<RegisterInfoPtr> m_registerList;

    std::vector<OperationInfoPtr> m_operationList;
};