// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2019 The ECODOLLAR developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h" 
#include "chainparams.h"
#include "consensus/merkle.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>
#include <limits>


struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("7c24cab77a7215bdf226411fcb1dd155d2f1bed81b6c46ed6a5fc089c3e84a8c"))
	(50, uint256("000000beaf03964ed3a747d61a7bb2d2e0426aec89bd14df8cd84a4dea725280"))
    (100, uint256("000000e4463c8c06b59e46c31a899dd53a6e60fb54c48517bbb87514fdbd591a"))
    (500, uint256("054ca6ecc233924a55d9d9330b1ce020ce53ceea0b32d2a193426c95798b15b4"))
    (1000, uint256("b911363699c63dea36a02aa348deaaa70a4cbcbd7e750c911ad9b33ca5363cb0")) 
    (2500, uint256("395f8b096b765bea64554d36fb404b359e96eac46de6764b53b95b0f122a4c83"))
    (5000, uint256("f1bd014723a7cc689b86e27e91213ad7d8c00200cf91a7becf812c116fe57dc8"))
	(10000, uint256("6acfe9f817d5c8413af6e6b3b1d931916b9907474a39495af7884aebbf6ea69b"))
    //(20000, uint256(""))
    //(40000, uint256(""))
    ;
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1586835510, // * UNIX timestamp of last checkpoint block
    5116987,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256("1828db2e65f87f58181327b123b5642528f82a9ca500e1df3e43b2eb3eb5c0fb"))
	(50, uint256("000000635d5ab9577482af29e4a1ebec426291e8fe7c546e46a6caea567ea805"))
    (100, uint256("000000860d04762298f12b9cf1b9f71bdfaf1adf6bbb4fc29bdf02243c5f885b"))
    (500, uint256("71493beafa9987ce8d93cd260144d92310583573d2d222e4692c12ba72972a9e"))
    (1000, uint256("9458d413723cef871d7e5f9e1463d639feea94a316459e8bcf39875c94433229")) 
    (2500, uint256("a8b65ae8699297794b4b1cb0e3de856530e45da8c522e9af1678c6664b3f4b6a"))
    //(5000, uint256(""))
	//(10000, uint256(""))
    //(20000, uint256(""))
    //(40000, uint256(""))    
    ;
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1587053940,
    5116987,
    2000};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1454124731,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

bool CChainParams::HasStakeMinAgeOrDepth(const int contextHeight, const uint32_t contextTime,
        const int utxoFromBlockHeight, const uint32_t utxoFromBlockTime) const
{
    // before stake modifier V2, the age required was 60 * 60 (1 hour). Not required for regtest
    if (!IsStakeModifierV2(contextHeight))
        return NetworkID() == CBaseChainParams::REGTEST || (utxoFromBlockTime + nStakeMinAge <= contextTime);

    // after stake modifier V2, we require the utxo to be nStakeMinDepth deep in the chain
    return (contextHeight - utxoFromBlockHeight >= nStakeMinDepth);
}

int CChainParams::FutureBlockTimeDrift(const int nHeight) const
{
    if (IsTimeProtocolV2(nHeight))
        // PoS (TimeV2): 14 seconds
        return TimeSlotLength() - 1;

    // PoS (TimeV1): 3 minutes
    // PoW: 2 hours
    return (nHeight > LAST_POW_BLOCK()) ? nFutureTimeDriftPoS : nFutureTimeDriftPoW;
}

bool CChainParams::IsValidBlockTimeStamp(const int64_t nTime, const int nHeight) const
{
    // Before time protocol V2, blocks can have arbitrary timestamps
    if (!IsTimeProtocolV2(nHeight))
        return true;

    // Time protocol v2 requires time in slots
    return (nTime % TimeSlotLength()) == 0;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x91;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0xea;
        vAlertPubKey = ParseHex("04f526f5087b37c392fda9e840efd6fbda3165ccb65c0867ac329a68705479385872767ac47fe956db5baa8ee076a3be619bc500fbfc6e39b2bef53e89aff2aa5f");
        nDefaultPort = 39023;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // ECODOLLAR starting difficulty is 1 / 2^12
        bnProofOfStakeLimit = ~uint256(0) >> 24;
        bnProofOfStakeLimit_V2 = ~uint256(0) >> 20; // 60/4 = 15 ==> use 2**4 higher limit
        nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 8100; // 75%
        nRejectBlockOutdatedMajority = 10260; // 95%
        nToCheckBlockUpgradeMajority = 10800; // Approximate expected amount of blocks in 7 days (1440*7.5)
        nMinerThreads = 0;
        nTargetSpacing = 1 * 60;                        // 1 minute
        nTargetTimespan = 40 * 60;                      // 40 minutes
        nTimeSlotLength = 15;                           // 15 seconds
        nTargetTimespan_V2 = 2 * nTimeSlotLength * 60;  // 30 minutes
        nMaturity = 100;
        nStakeMinAge = 60 * 60;                         // 1 hour
        nStakeMinDepth = 100;
        nFutureTimeDriftPoW = 7200;
        nFutureTimeDriftPoS = 180;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 0x7FFFFFFFFFFFFFFF;
        nMinColdStakingAmount = 1 * COIN;
        nMNCollateral = 100000 * COIN;
        strBackboneAddr = "EXEEmiM5LDhXfWYMmGB2t7yKrWsCWAkEyQ";

        /** Height or Time Based Activations **/
        nLastPOWBlock = 400;
        nEcodollarBadBlockTime = 1471401614; // Skip nBit validation of Block 259201 per PR #915
        nEcodollarBadBlocknBits = 0x1c056dac; // Skip nBit validation of Block 259201 per PR #915
        nModifierUpdateBlock = 401;
        nZerocoinStartHeight = 401;
        nZerocoinStartTime = 1577858523; // January 01, 2020 6:02:00 AM
        nBlockEnforceSerialRange = 401; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = 401; //Trigger a recalculation of accumulators 
        nBlockFirstFraudulent = 401; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 401; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = 401; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = 0;
        nBlockZerocoinV2 = 401; //!> The block that zerocoin v2 becomes active
        nBlockDoubleAccumulated = 401;
        nEnforceNewSporkKey = 1566860400; //!> Sporks signed after Monday, August 26, 2019 11:00:00 PM GMT must use the new spork key
        nRejectOldSporkKey = 1569538800; //!> Fully reject old spork key after Thursday, September 26, 2019 11:00:00 PM GMT
        nBlockStakeModifierlV2 = 401;
        nBIP65ActivationHeight = 401;
        nBlockTimeProtocolV2 = 401;
        nPublicZCSpends = 401;

        // New P2P messages signatures
        nBlockEnforceNewMessageSignatures = nBlockTimeProtocolV2;

        // Blocks v7
        nBlockLastAccumulatorCheckpoint = 401;
        nBlockV7StartHeight = nBlockTimeProtocolV2;

        // Fake Serial Attack
        nFakeSerialBlockheightEnd = -1;
        nSupplyBeforeFakeSerial = 0 * COIN;   // zerocoin supply at block nFakeSerialBlockheightEnd

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         */
        const char* pszTimestamp = "EcoDollar Genesis The Dow soars as coronavirus slows: April 6, 2020";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04c1593ebdb94cb3eb9d4939ae0da735a36f7e35947f4e7097ede91ac3638e60a596e5007a0f0ca95322ea75bdf2ab628d53790f057c9b422be0a5a0367c764303") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
        genesis.nVersion = 1;
        genesis.nTime = 1586235037;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 2252147;        

        hashGenesisBlock = genesis.GetHash();
		
		//printf("block.GetHash = %s\n", genesis.GetHash().ToString().c_str());
		//printf("block.Merkle = %s\n", genesis.hashMerkleRoot.ToString().c_str());
        //printf("block.NBits = %s\n", genesis.nBits.ToString().c_str());
         
        assert(hashGenesisBlock == uint256("0x7c24cab77a7215bdf226411fcb1dd155d2f1bed81b6c46ed6a5fc089c3e84a8c"));
        assert(genesis.hashMerkleRoot == uint256("0xa641ff4c8a3adc95064325a3caa718a6764b8d303c716e2ea27934416edc2777"));

        vSeeds.push_back(CDNSSeedData("ecodollar.org", "seed01.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "seed02.ecodolar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodollar.org", "seed03.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "seed04.ecodolar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodollar.org", "seed05.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "seed06.ecodolar.org")); // Primary DNS Seeder from EcoZion

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 33);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 15);
        base58Prefixes[STAKING_ADDRESS] = std::vector<unsigned char>(1, 63); // starting with 'S'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 201);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        nBudgetCycleBlocks = 43200; //!< Amount of blocks in a months period of time (using 1 minutes per) = (60*24*30)
        strSporkPubKey = "040300c0ea140880b04bb2d9dd815f6a04bc18222c912b1b06b5169f3fc59ba592046ab87dddb7e5ea17608b71613e3d8de7b79a34cea8851789bbddfda83ef83c";
        strSporkPubKeyOld = "04188e1294141598eba066531b1fd3890e73374d8c675c7bdceb4014189f3eaca8cffb551a2043b441ebbbd1fe5e2552d9a4c389670df25000dbd039e4fcb59bb0";
        strObfuscationPoolDummyAddress = "D87q2gC9j6nNrnzCsg4aY6bHMLsT9nUhEw";
        nStartMasternodePayments = 1403728576; //Wed, 25 Jun 2014 20:36:16 GMT

        /** Zerocoin */
        zerocoinModulus = "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMaxZerocoinPublicSpendsPerTransaction = 637; // Assume about 220 bytes each input
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nZerocoinRequiredStakeDepth = 200; //The required confirmations for a zecos to be stakable

        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
        nProposalEstablishmentTime = 60 * 60 * 24; // Proposals must be at least a day old to make it into a budget
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }

};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x45;
        pchMessageStart[1] = 0x76;
        pchMessageStart[2] = 0x65;
        pchMessageStart[3] = 0xba;
        vAlertPubKey = ParseHex("049ca19936c46d286676ca27002105cdc5921fe5d2d6fb98cd2d4a143e4f5d37601ebc380595772bef6618e45ade567c905b4c7ce51d47a734dd8ade38412a8dcc");
        nDefaultPort = 39025;
        nEnforceBlockUpgradeMajority = 4320; // 75%
        nRejectBlockOutdatedMajority = 5472; // 95%
        nToCheckBlockUpgradeMajority = 5760; // 4 days
        nMinerThreads = 0;
        nLastPOWBlock = 400;
        nEcodollarBadBlockTime = 1489001494; // Skip nBit validation of Block 259201 per PR #915
        nEcodollarBadBlocknBits = 0x1e0a20bd; // Skip nBit validation of Block 201 per PR #915
        nMaturity = 15;
        nStakeMinDepth = 100;
        nMasternodeCountDrift = 4;
        //nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nModifierUpdateBlock = 401;
        nMaxMoneyOut = 0x7FFFFFFFFFFFFFFF;
        nMinColdStakingAmount = 1 * COIN;
        nMNCollateral = 100000 * COIN;
        strBackboneAddr = "y94hEK3C5JS4dYEfu4QW7XWF5QHNBqiyow";

        //nZerocoinStartHeight = 201576;
        nZerocoinStartHeight = 401;
        nZerocoinStartTime = 1501776000;
        nBlockEnforceSerialRange = 401; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = 401; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = 401; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 401; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = 401; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = 0; //Amount of invalid coins filtered through exchanges, that should be considered valid
        nBlockZerocoinV2 = 401; //!> The block that zerocoin v2 becomes active
        nEnforceNewSporkKey = 1566860400; //!> Sporks signed after Monday, August 26, 2019 11:00:00 PM GMT must use the new spork key
        nRejectOldSporkKey = 1569538800; //!> Reject old spork key after Thursday, September 26, 2019 11:00:00 PM GMT
        nBlockStakeModifierlV2 = 401;
        nBIP65ActivationHeight = 401;
        // Activation height for TimeProtocolV2, Blocks V7 and newMessageSignatures
        //nBlockTimeProtocolV2 = 1347000;
        nBlockTimeProtocolV2 = 401;

        // Public coin spend enforcement
        nPublicZCSpends = 401;

        // New P2P messages signatures
        nBlockEnforceNewMessageSignatures = nBlockTimeProtocolV2;

        // Blocks v7
        nBlockLastAccumulatorCheckpoint = nPublicZCSpends - 10;
        nBlockV7StartHeight = nBlockTimeProtocolV2;

        // Fake Serial Attack
        nFakeSerialBlockheightEnd = -1;
        nSupplyBeforeFakeSerial = 0;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.

		
        genesis.nTime = 1586829680;
        genesis.nNonce = 2252147;     

        hashGenesisBlock = genesis.GetHash();

		//printf("block.GetHash = %s\n", genesis.GetHash().ToString().c_str());
		//printf("block.Merkle = %s\n", genesis.hashMerkleRoot.ToString().c_str());
        //printf("block.NBits = %s\n", genesis.nBits.ToString().c_str());

        assert(hashGenesisBlock == uint256("0x1828db2e65f87f58181327b123b5642528f82a9ca500e1df3e43b2eb3eb5c0fb"));
		
        vFixedSeeds.clear();
        vSeeds.clear();
        
        vSeeds.push_back(CDNSSeedData("ecodollar.org", "testseed01.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "testseed02.ecodolar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodollar.org", "testseed03.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "testseed04.ecodolar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodollar.org", "testseed05.ecodollar.org")); // Primary DNS Seeder from EcoZion
        vSeeds.push_back(CDNSSeedData("ecodolar.org", "testseed06.ecodolar.org")); // Primary DNS Seeder from EcoZion

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 139); // Testnet pivx addresses start with 'x' or 'y'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet pivx script addresses start with '8' or '9'
        base58Prefixes[STAKING_ADDRESS] = std::vector<unsigned char>(1, 73);     // starting with 'W'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet pivx BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet pivx BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet pivx BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();
        
        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        nBudgetCycleBlocks = 144; //!< Ten cycles per day on testnet
        strSporkPubKey = "04bf8e29dfe9fa58db660c088fe23f99a08d67d7e56316d49594bcb68d53eebe3e2bdd19e44e86e266320fa2060bf063cc5d7010226ebbf71aae14babc344a0600";
        strSporkPubKeyOld = "04745cb524aaa912d8fe15c8ce840504200fc76bc09169b7bfb3d982ca27bec51c90e007e004ee942b3058ef4d926bc5ae64462ea95592d2f565b73a2bbd602518";
        strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet

        nProposalEstablishmentTime = 60 * 5; // Proposals must be at least 5 mns old to make it into a test budget
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xa1;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xac;
        nDefaultPort = 51476;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nLastPOWBlock = 250;
        nMaturity = 100;
        nStakeMinAge = 0;
        nStakeMinDepth = 0;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 0;
        nMaxMoneyOut = 43199500 * COIN;
        nZerocoinStartHeight = 300;
        nBlockZerocoinV2 = 300;
        nZerocoinStartTime = 1501776000;
        nBlockEnforceSerialRange = 1;               // Enforce serial range starting this block
        nBlockRecalculateAccumulators = 999999999;  // Trigger a recalculation of accumulators
        nBlockFirstFraudulent = 999999999;          // First block that bad serials emerged
        nBlockLastGoodCheckpoint = 999999999;       // Last valid accumulator checkpoint
        nBlockStakeModifierlV2 = nLastPOWBlock + 1; // start with modifier V2 on testnet
        nBlockTimeProtocolV2 = 999999999;

        nMintRequiredConfirmations = 10;
        nZerocoinRequiredStakeDepth = nMintRequiredConfirmations;

        // Public coin spend enforcement
        nPublicZCSpends = 400;

        // Blocks v7
        nBlockV7StartHeight = nPublicZCSpends + 1;
        nBlockLastAccumulatorCheckpoint = nPublicZCSpends - 10;

        // New P2P messages signatures
        nBlockEnforceNewMessageSignatures = 1;

        // Fake Serial Attack
        nFakeSerialBlockheightEnd = -1;

        //! Modify the regtest genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1586235037;
        genesis.nNonce = 2252147;     

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x7c24cab77a7215bdf226411fcb1dd155d2f1bed81b6c46ed6a5fc089c3e84a8c"));
        
        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        /* Spork Key for RegTest:
        WIF private key: 932HEevBSujW2ud7RfB1YF91AFygbBRQj3de3LyaCRqNzKKgWXi
        private key hex: bd4960dcbd9e7f2223f24e7164ecb6f1fe96fc3a416f5d3a830ba5720c84b8ca
        Address: yCvUVd72w7xpimf981m114FSFbmAmne7j9
        */
        strSporkPubKey = "043969b1b0e6f327de37f297a015d37e2235eaaeeb3933deecos8162c075cee0207b13537618bde640879606001a8136091c62ec272dd0133424a178704e6e75bb7";
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;

static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
