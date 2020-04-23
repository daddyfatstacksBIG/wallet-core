// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"

#include "../Hash.h"

using namespace TW;
using namespace TW::Waves;

Proto::SigningOutput Signer::sign(const Proto::SigningInput& input) noexcept {
    auto privateKey = PrivateKey(Data(input.private_key().begin(), input.private_key().end()));
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeCURVE25519);
    auto transaction = Transaction(input, publicKey.bytes);

    Data signature = Signer::sign(privateKey, transaction);

    Proto::SigningOutput output = Proto::SigningOutput();
    output.set_signature(reinterpret_cast<const char*>(signature.data()), signature.size());
    output.set_json(transaction.buildJson(signature).dump());
    return output;
}

Data Signer::sign(const PrivateKey& privateKey, Transaction& transaction) noexcept {
    try {
        auto bytesToSign = transaction.serializeToSign();
        auto signature = privateKey.sign(bytesToSign, TWCurveCurve25519);
        return signature;
    } catch (...) {
        return Data();
    }
}
