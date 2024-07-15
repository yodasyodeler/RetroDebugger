#pragma once

#include "RetroDebuggerCommon.h"

#include <functional>

namespace Rdb {
using GetProgramCounterFunc = std::function<unsigned int()>;

using ReadMemoryFunc = std::function<unsigned int(unsigned int)>;

using ReadBankableMemoryFunc = std::function<unsigned int(BankNum, unsigned int)>;

using CheckBankableMemoryLocationFunc = std::function<bool(BankNum, unsigned int)>;

using GetRegSetFunc = std::function<RegSet()>;
}