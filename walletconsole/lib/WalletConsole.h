// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include "Buffer.h"
#include "Coins.h"
#include "CommandExecutor.h"
#include "Keys.h"

#include "Data.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace TW::WalletConsole {

using namespace std;

class WalletConsole {
  protected:
    istream& _inst;
    ostream& _outst;
    CommandExecutor _executor;

  public:
    WalletConsole(istream& ins, ostream& outs) : _inst(ins), _outst(outs), _executor(outs) {
        init();
    }
    void init();
    void loop();
    static bool isExit(const string& cmd);
};

} // namespace TW::WalletConsole
