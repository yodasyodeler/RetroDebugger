
#include "DebuggerOperations.h"
#include "DebuggerXmlParser.h"

#include "DebuggerCallbacks.h"

#include <algorithm>
#include <memory>

DebuggerOperations::DebuggerOperations(const std::shared_ptr<DebuggerXmlParser>& debuggerParser) :
    m_debuggerParser(debuggerParser) {}


DebuggerOperations::DebuggerOperations(const std::shared_ptr<DebuggerXmlParser>& debuggerParser, const std::string& filename) :
    m_debuggerParser(debuggerParser) {
    ParseFile(filename);
}

void DebuggerOperations::Reset() {
    m_operations.operations.clear();
    m_operations.extendedOperations.clear();
    m_operations.opcodeLength = 0;
    m_jumpOperations.operations.clear();
    m_jumpOperations.extendedOperations.clear();
    m_jumpOperations.opcodeLength = 0;
}

bool DebuggerOperations::IsValid() {
    return m_isValid;
}

std::string DebuggerOperations::GetErrorMessage() {
    return m_debuggerParser->GetLastError();
}

Operations DebuggerOperations::GetOperations() {
    return m_operations;
}

Operations DebuggerOperations::GetJumpOpertions() {
    return m_jumpOperations;
}

std::vector<RegisterInfoPtr> DebuggerOperations::GetRegisters() {
    return m_registerList;
}

// TODO: optimize, there is some repeated stuff here
size_t DebuggerOperations::GetOperation(size_t address, Operation& operation) {
    const auto opcode1 = DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++));

    if (opcode1 >= (1u << m_operations.opcodeLength)) {
        auto name = std::to_string(opcode1);
        operation.info = std::make_shared<OperationInfo>(name, false);
        return 1;
    } // TODO: add error info, opcode length is too great

    auto re = 1u; // opcode
    if (m_operations.operations.find(opcode1) != m_operations.operations.end()) {
        operation = m_operations.operations.at(opcode1);
        for (const auto& arg : operation.arguments) {
            // TODO: immediate values a bit hacky, assumes a byte being read back
            const auto argLength = GetArgTypeLength(arg->type);
            for (auto i = 0u; i < argLength; ++i) {
                constexpr auto byteSize = 8u;
                const auto bitShift = byteSize * i;
                arg->operationValue |= DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++)) << bitShift;
            }
            re += argLength;
        }
    }
    else if (m_operations.extendedOperations.find(opcode1) != m_operations.extendedOperations.end()) {
        const auto extOperations = m_operations.extendedOperations.at(opcode1);
        const auto opcode2 = DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++));
        re += 1; // extended opcode
        operation = extOperations.at(opcode2);
        for (const auto& arg : operation.arguments) {
            // TODO: immediate values a bit hacky, assumes a byte being read back
            const auto argLength = GetArgTypeLength(arg->type);
            for (auto i = 0u; i < argLength; ++i) {
                constexpr auto byteSize = 8u;
                const auto bitShift = byteSize * i;
                arg->operationValue |= DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++)) << bitShift;
            }
            re += argLength;
        }
    }
    else {
        auto name = std::to_string(opcode1);
        operation.info = std::make_shared<OperationInfo>(name, false);
    }

    return re;
}

bool DebuggerOperations::ParseFile(const std::string& filename) {
    m_isValid = m_debuggerParser->ParseFile(filename);
    if (!m_isValid) {
        return false;
    }

    const auto xmlOperations = m_debuggerParser->GetOperations();
    for (const auto& operations : xmlOperations) {
        if (operations.first == NORMAL_OPERATIONS_KEY) {
            for (const auto& operation : operations.second.operations) {
                ConvertOperation(m_operations.operations, operation.second);
            }
            m_operations.opcodeLength = operations.second.opcodeLength;
        }
        else {
            OpcodeToOperation operationsMap;
            for (const auto& operation : operations.second.operations) {
                ConvertOperation(operationsMap, operation.second);
            }
            m_operations.extendedOperations.emplace(operations.first, operationsMap);
        }
    }
    m_debuggerParser->Reset();

    m_jumpOperations.opcodeLength = m_operations.opcodeLength;
    for (const auto& operation : m_operations.operations) {
        if (operation.second.info->isJump) {
            m_jumpOperations.operations.emplace(operation);
        }
    }
    for (const auto& operations : m_operations.extendedOperations) {
        for (const auto& operation : operations.second) {
            if (operation.second.info->isJump) {
                m_jumpOperations.extendedOperations.emplace(operations);
            }
        }
    }

    return m_isValid;
}

// TODO: clean this up.
void DebuggerOperations::ConvertOperation(OpcodeToOperation& operationMap, const XmlDebuggerOperation& xmlOperation) {
    const auto findMatchingOperationName = [xmlOperation](const std::shared_ptr<OperationInfo>& operation) -> bool { return operation->name == xmlOperation.command; };


    OperationInfoPtr operationInfoPtr;
    const auto findResult = std::find_if(m_operationList.begin(), m_operationList.end(), findMatchingOperationName);
    if (findResult == m_operationList.end()) {
        auto operationInfo = std::make_shared<OperationInfo>(xmlOperation.command, xmlOperation.isJump);

        operationInfoPtr = m_operationList.emplace_back(operationInfo);
    }
    else {
        operationInfoPtr = *findResult;
    }

    std::vector<ArgumentPtr> operationArguments;
    for (const auto& arg : xmlOperation.arguments) {
        Argument argument = { arg.type, arg.indirectArg, arg.operation, arg.value.offset, arg.value.name, arg.value.reg };

        ArgumentPtr argumentPtr;
        const auto argumentFind = std::find_if(m_argumentList.begin(), m_argumentList.end(), [argument](const ArgumentPtr& arg) -> bool { return argument == *arg; });
        if (argumentFind == m_argumentList.end()) {
            auto argumentInfo = std::make_shared<Argument>(argument);
            argumentPtr = m_argumentList.emplace_back(argumentInfo);
        }
        else {
            argumentPtr = *argumentFind;
        }
        const RegisterInfo reg = { argumentPtr->reg };
        operationArguments.emplace_back(argumentPtr);
        if (!reg.name.empty() && std::find_if(m_registerList.begin(), m_registerList.end(), [&](const RegisterInfoPtr& regInfo) { return reg.name == regInfo->name; }) == m_registerList.end()) {
            m_registerList.emplace_back(std::make_shared<RegisterInfo>(reg));
        }
    }

    auto opcode = xmlOperation.opcode;
    Operation operation = { operationInfoPtr, operationArguments };
    operationMap.emplace(opcode, operation);
}
