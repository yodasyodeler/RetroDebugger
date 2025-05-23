#pragma once

#include "RetroDebuggerCommon.h"

#include <limits>

enum DbgRegName {};

struct RegInfo
{
    DbgRegName reg;
    unsigned int value;
};

enum OpcodeImmediateType {
    None = 0,
    Immediate8Bit,
    Immediate16Bit,
    IoAddress8Bit,
    IoAddressSource8Bit,
    Address16Bit,
    AddressSource16Bit,
    SignedOffset8Bit,
};

struct AddrInfo
{
    unsigned int address;
    unsigned int value;
};

// struct CommandInfo
// {
//     unsigned int address;
//     unsigned int commandOp;
//     unsigned int immediate;
//     OpcodeImmediateType immediateType;
//     bool isExtendedCommand;
//     unsigned int sizeOfCmd;
// };

struct comp
{
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return lhs.compare(rhs.c_str()) < 0;
    }
};

// TODO: clean this up
// TODO: there is overlap with other structs here, need to clean up xml and then merge the ideas
// TODO: remove AUTO, its not needed and should never be the end type
enum class ArgumentType : int {
    UNKOWN = 0,
    AUTO,
    CONSTANT,
    REG,
    CONDITIONAL,
    S8BIT,
    S16BIT,
    S32BIT,
    U8BIT,
    U16BIT,
    U32BIT
};

enum class RegOperationType : int {
    NONE = 0,
    POSTINC,
    POSTDEC,
    PREINC,
    PREDEC,
    REG_OFFSET_ADD,
    REG_OFFSET_SUB,
    REG_OFFSET_U8BIT,
    REG_OFFSET_U16BIT,
    REG_OFFSET_S8BIT,
    REG_OFFSET_S16BIT
};

typedef unsigned int OpcodeLength;

// TODO: is the Reset method a good idea?
struct XmlArgValue
{
    void Reset() {
        nameFirst = true;
        name.clear();
        reg.clear();
        offset = 0;
    }
    bool nameFirst = true; // REG+NUM or NUM+REG, TODO: may remove this, allows the ability to say which value was first, but isn't really that useful for most opcodes.
    std::string name;
    std::string reg;
    unsigned int offset{};
};

struct XmlDebuggerArgument
{
    void Reset() {
        type = ArgumentType::UNKOWN;
        indirectArg = false;
        operation = RegOperationType::NONE;
        value.Reset();
    }
    ArgumentType type = ArgumentType::UNKOWN;
    bool indirectArg = false;
    RegOperationType operation = RegOperationType::NONE;
    XmlArgValue value{}; // used in register lookup
};

static constexpr auto NormalOperationsKey = std::numeric_limits<unsigned int>::max();

struct XmlDebuggerOperation
{
    void Reset() {
        opcode = 0u;
        command.clear();
        arguments.clear();
        isJump = false;
    }
    unsigned int opcode{};
    std::string command;
    std::vector<XmlDebuggerArgument> arguments;
    bool isJump = false;
};
typedef std::map<unsigned int, XmlDebuggerOperation> XmlOpcodeToOperation;

struct XmlDebuggerOperations
{
    void Reset() {
        opcodeLength = 0;
        extendedOpcode = NormalOperationsKey;
        operations.clear();
    }
    OpcodeLength opcodeLength{};
    unsigned int extendedOpcode = NormalOperationsKey;
    XmlOpcodeToOperation operations;
};

typedef std::map<unsigned int, XmlDebuggerOperations> XmlOperationsMap;

// TODO: clean this up, there is overlap and stale
enum class ImmediateType : int {
    UNKOWN = -1,
    NONE = 0
}; // TODO: think about naming for this enum
enum class FlagType : int {
    UNKOWN = -1,
    NONE = 0,
    ZERO,
    SUBTRACT,
    HALF_CARRY,
    CARRY
};
enum class OperationType : int {
    UNKOWN = -1,
    NONE = 0,
    NOP,
    LOAD,
    ADD
};

struct DebuggerCommand
{
    std::string m_name;
    std::string m_mnemonicPrint;
    std::string m_prettyPrint;
    unsigned int m_numberArguments;
    OperationType m_operationType;
    ImmediateType m_immediateType;
    std::set<FlagType> m_flagsAffected;
    std::string m_description;
};

typedef std::shared_ptr<const DebuggerCommand> DebuggerCommandPtr;
typedef std::map<unsigned int, DebuggerCommandPtr> DebuggerCommandMap;

struct Argument
{
    inline bool operator==(const Argument& rhs) const {
        return (type == rhs.type) && (indirectArg == rhs.indirectArg) && (operation == rhs.operation) && (operationValue == rhs.operationValue) && (name == rhs.name);
    }
    ArgumentType type;
    bool indirectArg;
    RegOperationType operation;
    unsigned int operationValue;
    std::string name;
    std::string reg;
};
using ArgumentPtr = std::shared_ptr<Argument>;

// TODO: Consider adding error info
struct OperationInfo
{
    OperationInfo(std::string Name, bool IsJump) :
        name(Name), isJump(IsJump) {}
    std::string name;
    bool isJump;
};
using OperationInfoPtr = std::shared_ptr<OperationInfo>;

struct Operation
{
    OperationInfoPtr info;
    std::vector<ArgumentPtr> arguments;
};

typedef std::map<unsigned int, Operation> OpcodeToOperation;
typedef std::map<unsigned int, struct Operations> ExtendedOpcodeToOperation;
struct Operations
{
    OpcodeLength opcodeLength = 8;
    OpcodeToOperation operations = {};
    ExtendedOpcodeToOperation extendedOperations = {};
};

inline unsigned int GetArgTypeLength(ArgumentType argType) {
    switch (argType) {
        case ArgumentType::S8BIT:
        case ArgumentType::U8BIT:
            return 1;
        case ArgumentType::S16BIT:
        case ArgumentType::U16BIT:
            return 2;
        case ArgumentType::S32BIT:
        case ArgumentType::U32BIT:
            return 4;
        default:
            return 0; // TODO: Should this fail?
    }
}

using CommandList = std::map<size_t, Operation>;