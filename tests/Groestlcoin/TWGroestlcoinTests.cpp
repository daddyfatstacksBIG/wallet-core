// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../interface/TWTestUtilities.h"

#include <TrustWalletCore/TWAnyAddress.h>
#include <TrustWalletCore/TWBitcoinScript.h>
#include <TrustWalletCore/TWGroestlcoinAddress.h>
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWHRP.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWPrivateKey.h>

#include <gtest/gtest.h>

TEST(Groestlcoin, Address) {
    auto privateKey =
        WRAP(TWPrivateKey,
             TWPrivateKeyCreateWithData(
                 DATA("3c3385ddc6fd95ba7282051aeb440bc75820b8c10db5c83c052d7586e3e98e84").get()));
    auto publicKey = TWPrivateKeyGetPublicKeySecp256k1(privateKey.get(), true);
    auto address =
        WRAP(TWGroestlcoinAddress, TWGroestlcoinAddressCreateWithPublicKey(
                                       publicKey, TWCoinTypeP2pkhPrefix(TWCoinTypeGroestlcoin)));
    auto addressString = WRAPS(TWGroestlcoinAddressDescription(address.get()));
    assertStringsEqual(addressString, "Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM");

    auto address2 = WRAP(
        TWGroestlcoinAddress,
        TWGroestlcoinAddressCreateWithString(STRING("Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM").get()));
    auto address2String = WRAPS(TWGroestlcoinAddressDescription(address2.get()));
    assertStringsEqual(address2String, "Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM");

    ASSERT_TRUE(TWGroestlcoinAddressEqual(address.get(), address2.get()));
}

TEST(Groestlcoin, BuildForLegacyAddress) {
    auto script =
        WRAP(TWBitcoinScript,
             TWBitcoinScriptBuildForAddress(STRING("Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM").get(),
                                            TWCoinTypeGroestlcoin));
    auto scriptData = WRAPD(TWBitcoinScriptData(script.get()));
    assertHexEqual(scriptData, "76a91498af0aaca388a7e1024f505c033626d908e3b54a88ac");
}

TEST(Groestlcoin, BuildForSegwitP2SHAddress) {
    auto script =
        WRAP(TWBitcoinScript,
             TWBitcoinScriptBuildForAddress(STRING("31inaRqambLsd9D7Ke4USZmGEVd3PHkh7P").get(),
                                            TWCoinTypeGroestlcoin));
    auto scriptData = WRAPD(TWBitcoinScriptData(script.get()));
    assertHexEqual(scriptData, "a9140055b0c94df477ee6b9f75185dfc9aa8ce2e52e487");
}

TEST(Groestlcoin, BuildForNativeSegwitAddress) {
    auto script = WRAP(
        TWBitcoinScript,
        TWBitcoinScriptBuildForAddress(STRING("grs1qw4teyraux2s77nhjdwh9ar8rl9dt7zww8r6lne").get(),
                                       TWCoinTypeGroestlcoin));
    auto scriptData = WRAPD(TWBitcoinScriptData(script.get()));
    assertHexEqual(scriptData, "00147557920fbc32a1ef4ef26bae5e8ce3f95abf09ce");
}

TEST(Groestlcoin, ExtendedKeys) {
    auto wallet =
        WRAP(TWHDWallet, TWHDWalletCreateWithMnemonic(
                             STRING("all all all all all all all all all all all all").get(),
                             STRING("").get()));

    // .bip44
    auto xprv = WRAPS(TWHDWalletGetExtendedPrivateKey(wallet.get(), TWPurposeBIP44,
                                                      TWCoinTypeGroestlcoin, TWHDVersionXPRV));
    auto xpub = WRAPS(TWHDWalletGetExtendedPublicKey(wallet.get(), TWPurposeBIP44,
                                                     TWCoinTypeGroestlcoin, TWHDVersionXPUB));

    assertStringsEqual(xprv, "xprv9zHDfLCJPTf5UrS16CrJ56WzSSoAYhJriX8Lfsco3TtPhG2DkwkVXjaDxZKU5USfm"
                             "q5xjp1CZhpSrpHAPFwZWN75egb19TxWmMMmkd3csxP");
    assertStringsEqual(xpub, "xpub6DGa4qjCDqDNhLWUCEPJSETizUdexA2i5k3wUG2QboRNa4MNJV4k5XthorGcogStY"
                             "5K5iJ6NHtsznNK599ir8PmA3d1jqEoZHsixDTddNA9");

    // .bip49
    auto yprv = WRAPS(TWHDWalletGetExtendedPrivateKey(wallet.get(), TWPurposeBIP49,
                                                      TWCoinTypeGroestlcoin, TWHDVersionYPRV));
    auto ypub = WRAPS(TWHDWalletGetExtendedPublicKey(wallet.get(), TWPurposeBIP49,
                                                     TWCoinTypeGroestlcoin, TWHDVersionYPUB));

    assertStringsEqual(yprv, "yprvAJkRD9AD6QrU1hvSdcJT1Cdc1DwEMsBHFt4Gqd5NsK8Vhdn3ArEHYGaJhWotcn24V"
                             "Wx9rC6dDutHNea9zws8owL1qWEt3pVD2GGk4DSXyvm");
    assertStringsEqual(ypub, "ypub6Xjmceh6vnQmEBzujdqTNLaLZFmimKu8d6yse1UzRefUaS7BiPYY64tnYpQQydp1g"
                             "nb2cGkccBd1RtHRDtGXagqmRLxTStV88GWaeYh8ndG");

    // .bip84
    auto zprv = WRAPS(TWHDWalletGetExtendedPrivateKey(wallet.get(), TWPurposeBIP84,
                                                      TWCoinTypeGroestlcoin, TWHDVersionZPRV));
    auto zpub = WRAPS(TWHDWalletGetExtendedPublicKey(wallet.get(), TWPurposeBIP84,
                                                     TWCoinTypeGroestlcoin, TWHDVersionZPUB));
    assertStringsEqual(zprv, "zprvAcXuP1BeFt59rhLMnqTEL9j2TUz5mzXkj8NPcfvLKGzHm5mofJAeJMvFzzbNizahK"
                             "xVEvptBpSxdhBcGbxdbaFP58caWLWAjZWMT7Jb6pFW");
    assertStringsEqual(zpub, "zpub6qXFnWiY6FdT5BQptrzEhHfm1WpaBTFc6MHzR4KwscXGdt6xCqUtrAEjrHdeEsjaY"
                             "EwVMgjtTvENQ83yo2fmkYYGjTpJoH7vFWKQJp1bg1X");
}

TEST(Groestlcoin, DeriveFromZpub) {
    auto zpub = STRING("zpub6qXFnWiY6FdT5BQptrzEhHfm1WpaBTFc6MHzR4KwscXGdt6xCqUtrAEjrHdeEsjaYEwVMgj"
                       "tTvENQ83yo2fmkYYGjTpJoH7vFWKQJp1bg1X");
    auto pubKey4 = TWHDWalletGetPublicKeyFromExtended(zpub.get(), STRING("m/84'/17'/0'/0/4").get());
    auto pubKey11 =
        TWHDWalletGetPublicKeyFromExtended(zpub.get(), STRING("m/84'/17'/0'/0/11").get());

    auto address4 =
        WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(pubKey4, TWCoinTypeGroestlcoin));
    auto address4String = WRAPS(TWAnyAddressDescription(address4.get()));

    auto address11 =
        WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(pubKey11, TWCoinTypeGroestlcoin));
    auto address11String = WRAPS(TWAnyAddressDescription(address11.get()));

    assertStringsEqual(address4String, "grs1quwq6ml2r8rc25tue5ltfa6uc4pdzhtzul3c0rk");
    assertStringsEqual(address11String, "grs1ql0a7czm8wrj253h78dm2h5j2k89zwpy2qjq0q9");
}
