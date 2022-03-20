#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// TODO: should there be a common namespace here?

// TODO: should this be unsigned? Linux GDB has uses for internal breakpoints(signed). I don't believe this will though.
typedef unsigned int BreakNum;
typedef unsigned int BankNum;

enum class BreakType : unsigned int {
    Invalid = static_cast<unsigned int>(-1),
    Watchpoint = 0,
    Breakpoint,
    BankBreakpoint,
    Catchpoint,
};

enum class BreakDisposition {
    Keep = 0,
    Delete,
    Disable,
};

// TODO: Look into inline case statement and performance
static const std::map<BreakType, std::string> BreakTypeToString = {
    { BreakType::Watchpoint, "Watchpoint" },
    { BreakType::Breakpoint, "Breakpoint" },
    { BreakType::BankBreakpoint, "BankBreakpoint" },
    { BreakType::Catchpoint, "Catchpoint" },
};

static const std::map<BreakDisposition, std::string> BreakDispToString = {
    { BreakDisposition::Keep, "Keep" },
    { BreakDisposition::Delete, "Del " },
    { BreakDisposition::Disable, "Dis " },
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

enum DbgRegName {};

struct BreakInfo
{
    unsigned int address;
    BreakNum breakpointNumber;
    BankNum bankNumber;
    // unsigned int ignoreCount; //TODO: unimplemented
    // unsigned int enableCount; //TODO: unimplemented
    unsigned int timesHit;
    unsigned int oldWatchValue;
    unsigned int newWatchValue;
    BreakType type = BreakType::Invalid;
    BreakDisposition disp; // TODO: no way to use, is implemented though
    bool isEnabled;
};

struct RegInfo
{
    DbgRegName reg;
    unsigned int value;
};

struct AddrInfo
{
    unsigned int address;
    unsigned int value;
};

struct CommandInfo
{
    unsigned int address;
    unsigned int commandOp;
    unsigned int immediate;
    OpcodeImmediateType immediateType;
    bool isExtendedCommand;
    unsigned int sizeOfCmd;
};

struct comp
{
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

static const char* InstructionToString[0x100] = {};

static const char* ExtendedInstructionToString[0x100] = {};


// TODO: clean this up
// TODO: there is overlap with other structs here, need to clean up xml and then merge the ideas
// TODO: remove AUTO, its not needed and should never be the end type
enum class ArgumentType : int { UNKOWN = 0,
    AUTO,
    CONSTANT,
    REG,
    CONDITIONAL,
    S8BIT,
    S16BIT,
    S32BIT,
    U8BIT,
    U16BIT,
    U32BIT };

enum class RegOperationType : int { NONE = 0,
    POSTINC,
    POSTDEC,
    PREINC,
    PREDEC,
    REG_OFFSET_ADD,
    REG_OFFSET_SUB,
    REG_OFFSET_U8BIT,
    REG_OFFSET_U16BIT,
    REG_OFFSET_S8BIT,
    REG_OFFSET_S16BIT };

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
    bool nameFirst; // REG+NUM or NUM+REG, TODO: may remove this, allows the ability to say which value was first, but isn't really that useful for most opcodes.
    std::string name;
    std::string reg;
    unsigned int offset;
};

struct XmlDebuggerArgument
{
    void Reset() {
        type = ArgumentType::UNKOWN;
        indirectArg = false;
        operation = RegOperationType::NONE;
        value.Reset();
    }
    ArgumentType type;
    bool indirectArg;
    RegOperationType operation;
    XmlArgValue value; // used in register lookup
};

static const unsigned int NORMAL_OPERATIONS_KEY = UINT_MAX;

struct XmlDebuggerOperation
{
    void Reset() {
        opcode = 0u;
        command.clear();
        arguments.clear();
        isJump = false;
    }
    unsigned int opcode;
    std::string command;
    std::vector<XmlDebuggerArgument> arguments;
    bool isJump = false;
};
typedef std::map<unsigned int, XmlDebuggerOperation> XmlOpcodeToOperation;

struct XmlDebuggerOperations
{
    void Reset() {
        opcodeLength = 0;
        extendedOpcode = NORMAL_OPERATIONS_KEY;
        operations.clear();
    }
    OpcodeLength opcodeLength;
    unsigned int extendedOpcode;
    XmlOpcodeToOperation operations;
};

typedef std::map<unsigned int, XmlDebuggerOperations> XmlOperationsMap;


// TODO: clean this up, there is overlap and stale
enum class ImmediateType : int { UNKOWN = -1,
    NONE = 0 }; // TODO: think about naming for this enum
enum class FlagType : int { UNKOWN = -1,
    NONE = 0,
    ZERO,
    SUBTRACT,
    HALF_CARRY,
    CARRY };
enum class OperationType : int { UNKOWN = -1,
    NONE = 0,
    NOP,
    LOAD,
    ADD };

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

struct OperationInfo
{
    OperationInfo(std::string Name, bool IsJump) :
        name(Name), isJump(IsJump) {}
    std::string name;
    bool isJump;
};
using OpertionInfoPtr = std::shared_ptr<OperationInfo>;

struct Operation
{
    OpertionInfoPtr info;
    std::vector<ArgumentPtr> arguments;
};

typedef std::map<unsigned int, Operation> OpcodeToOperation;
typedef std::map<unsigned int, OpcodeToOperation> ExtendedOpcodeToOperation;
struct Operations
{
    OpcodeLength opcodeLength;
    OpcodeToOperation operations;
    ExtendedOpcodeToOperation extendedOperations;
};

inline unsigned int GetArgTypeLength(const ArgumentType& argType) {
    if (argType == ArgumentType::S8BIT) { return 1u; }
    else if (argType == ArgumentType::U8BIT) {
        return 1u;
    }
    else if (argType == ArgumentType::S16BIT) {
        return 2u;
    }
    else if (argType == ArgumentType::U16BIT) {
        return 2u;
    }
    else if (argType == ArgumentType::S32BIT) {
        return 4u;
    }
    else if (argType == ArgumentType::U32BIT) {
        return 4u;
    }
    else {
        return 0u;
    } // TODO: should this make it fail?
}


struct RegisterInfo
{
    std::string name;
};
using RegisterInfoPtr = std::shared_ptr<RegisterInfo>;

using RegSet = std::map<std::string, unsigned int>;

using CommandList = std::map<size_t, Operation>;
using BreakList = std::map<BreakNum, BreakInfo>;