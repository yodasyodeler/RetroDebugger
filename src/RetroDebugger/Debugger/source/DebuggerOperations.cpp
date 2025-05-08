
#include "DebuggerOperations.h"

#include "DebuggerCallbacks.h"

#include <algorithm>
#include <memory>
#include <ranges>

void DebuggerOperations::Reset() {
    m_operations = {};
    m_jumpOperations = {};
}

Operations DebuggerOperations::GetOperations() const {
    return m_operations;
}

Operations DebuggerOperations::GetJumpOperations() const {
    return m_jumpOperations;
}

std::vector<RegisterInfoPtr> DebuggerOperations::GetRegisters() const {
    return m_registerList;
}

// TODO: optimize, there is some repeated stuff here
size_t DebuggerOperations::GetOperation(size_t address, Operation& operation) {
    // Read the opcode
    static constexpr auto byteSize =  8U;
    const auto opcode1 = DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++));

    // Check for a bad opcode, "opcode greater than the emulated systems opcode length"
    // This is likely an error if the set up ReadMemory callback.
    // For now return the opcode number read and then return the expected size to go to the next address.
    if (opcode1 >= (1U << m_operations.opcodeLength)) {
        // TODO: add error info, opcode length is too great
        auto name = std::to_string(opcode1);
        operation.info = std::make_shared<OperationInfo>(name, false);
        return m_operations.opcodeLength;
    }

    // Check opcodes for the read opcode
    if (m_operations.operations.contains(opcode1)) {
        operation = m_operations.operations.at(opcode1);
        auto numOpcodes = (m_operations.opcodeLength / byteSize); // length in bytes
        for (const auto& arg : operation.arguments) {
            const auto argLength = GetArgTypeLength(arg->type);
            for (auto i = 0U; i < argLength; ++i) {
                const auto bitShift = byteSize * i;
                arg->operationValue |= DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++)) << bitShift;
            }
            numOpcodes += argLength;
        }
        return numOpcodes;
    }
    else if (m_operations.extendedOperations.contains(opcode1)) {
        const auto& extOperations = m_operations.extendedOperations.at(opcode1);
        const auto opcode2 = DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++));
        auto numOpcodes = (extOperations.opcodeLength * 2) / byteSize; // Extended Opcode and Opcode.
        operation = extOperations.operations.at(opcode2);

        // TODO: Chained extended Opcodes?
        for (const auto& arg : operation.arguments) {
            // TODO: immediate values a bit hacky, assumes a byte being read back
            //       Need to add the ability to specify ReadMemory callbacks size.
            const auto argLength = GetArgTypeLength(arg->type);
            for (auto i = 0U; i < argLength; ++i) {
                const auto bitShift = byteSize * i;
                arg->operationValue |= DebuggerCallback::ReadMemory(static_cast<unsigned int>(address++)) << bitShift;
            }
            numOpcodes += argLength;
        }
        return numOpcodes;
    }
    else {
        // Unrecognized opcode, may not be an error. Could be unrelated bytes being read from memory that don't correspond to a command.
        // Or an undefined command.
        auto name = std::to_string(opcode1);
        operation.info = std::make_shared<OperationInfo>(name, false);
        return 1;
    }
}

void DebuggerOperations::SetOperations(const XmlOperationsMap& XmlOperations) {
    for (const auto& [extensionOpcode, operationsInfo] : XmlOperations) {
        if (extensionOpcode == NormalOperationsKey) {
            for (const auto& operation : operationsInfo.operations | std::views::values) {
                ConvertOperation(m_operations.operations, operation);
            }
            m_operations.opcodeLength = operationsInfo.opcodeLength;
        }
        else {
            Operations operationsMap;
            for (const auto& operation : operationsInfo.operations | std::views::values) {
                ConvertOperation(operationsMap.operations, operation);
            }
            operationsMap.opcodeLength = operationsInfo.opcodeLength;
            m_operations.extendedOperations.emplace(extensionOpcode, operationsMap);
            // TODO: Chained extended Opcodes?
        }
    }

    // Rad through all opcodes and note the Jump opcodes
    m_jumpOperations.opcodeLength = m_operations.opcodeLength;
    for (const auto& operation : m_operations.operations) {
        if (operation.second.info->isJump) {
            m_jumpOperations.operations.emplace(operation);
        }
    }
    // TODO: Chained extended Opcodes?
    for (const auto& operations : m_operations.extendedOperations) {
        for (const auto& operation : operations.second.operations) {
            if (operation.second.info->isJump) {
                m_jumpOperations.extendedOperations.emplace(operations);
            }
        }
    }
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
        const Argument argument = { arg.type, arg.indirectArg, arg.operation, arg.value.offset, arg.value.name, arg.value.reg };

        ArgumentPtr argumentPtr;
        const auto argumentFind = std::find_if(m_argumentList.begin(), m_argumentList.end(), [argument](const ArgumentPtr& argPtr) -> bool { return argument == *argPtr; });
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
    const Operation operation = { operationInfoPtr, operationArguments };
    operationMap.emplace(opcode, operation);
}
