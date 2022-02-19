#pragma once

#include "DebuggerCommon.h"

class DebuggerXmlParser;

class DebuggerOperations {
public:
    DebuggerOperations(const std::shared_ptr<DebuggerXmlParser>& debuggerParser);
    DebuggerOperations(const std::shared_ptr<DebuggerXmlParser>& debuggerParser, const std::string& filename);

    void Reset();

    bool IsValid();
    std::string GetErrorMessage();
    Operations GetOperations();
    Operations GetJumpOpertions();
    std::vector<RegisterInfoPtr> GetRegisters();

    size_t GetOperation(size_t opcode, Operation& operation);
    bool ParseFile(const std::string& filename);

private:
    void ConvertOperation(OpcodeToOperation& operationMap, const XmlDebuggerOperation& xmlOperation);

    bool m_isValid = false;
    Operations m_operations = {};
    Operations m_jumpOperations = {};
    std::shared_ptr<DebuggerXmlParser> m_debuggerParser;
    std::vector<ArgumentPtr> m_argumentList;
    std::vector<RegisterInfoPtr> m_registerList;

    std::vector<OpertionInfoPtr> m_operationList;
};