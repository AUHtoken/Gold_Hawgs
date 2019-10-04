// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
	typedef std::map<int, uint256> MapCheckpoints;

	//
	// What makes a good checkpoint block?
	// + Is surrounded by blocks with reasonable timestamps
	//   (no blocks before with a timestamp after, none after with
	//    timestamp before)
	// + Contains no strange transactions
	//
	static MapCheckpoints mapCheckpoints =
		boost::assign::map_list_of
		(0, uint256("0x0000b7cef8199d7ebaf731bc3eebdcf878e5335fcf89a6acb294482b121c08d7"))
		(1, uint256("0xa0e2ea6b4fd64c67e4e8b330fedbf81489071dc39d303af3d50e9118fb679462"))
		(384, uint256("0x499fd9d267060d392bf61c4fe14a3099264da3d54af9d9786e4005d40f5ad198"))
		(813, uint256("0xa5f3bc5043e00aa34360c51057c4ffa243c2a41b857ff10dba22eb25c1ffedfb"))
		(1197, uint256("0x3674b8cf9f5b50bd9fb7c43992aeaad13b0bd06639ddf0f606f6fbef4a0fa01f"))
		(1601, uint256("0xee3e82b134c1c7934a2a703a63fa9f2685536cba69e42369c342668d99abbd75"))
		(1865, uint256("0x9b358fb20162ab7e894951fbbcfbc4bbece20f367d6552fd656f3eeec2d8f6e8"))
		(2098, uint256("0x0dd7900f5fc0e650f870bbeddb39733310f7228cf65e920045fcf12f4bc9050c"))
		(7845, uint256("0x87c9f81c9a46b4dcd7e8474988185091ee98887fbcc3cee5671392f5836dd4e7"))
		(16102, uint256("0x4a0a88559679d32e11ab6eacc5e5acc58bdb37bbb33af8c8dfc622ea77b17178"))
		(29813, uint256("0xdd5edde4395a7affc6111b6739d84ed05cc63430bd318c42d0c05adf7f37084a"))
		(36752, uint256("0x8e674424e8370b17e96337f4c966ec7dc041614bb88a96776fe2a40edff986df"))
		(43297, uint256("0xe587d9f1037a7d8f3cec5895c14e7f54f6f60c86bc6a592829186c970f3af9c7"))
		(51644, uint256("0x59a2d6fac9476f910f2fc032b4c509f755ada27d153ea93eeec6c79e8f41d7c8"))
		(64851, uint256("0x443f39a373f2dd1d112c04d1bf475b529a434855ab64198159a510d2e862f280"))
		(72115, uint256("0x9b2dfa7ca3065663eb3908f6ba60c34f5ade7bbf4642a6de84e0a4640a05e245"))	
	
		;

	// TestNet has no checkpoints
	static MapCheckpoints mapCheckpointsTestnet;

	bool CheckHardened(int nHeight, const uint256& hash)
	{
		MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

		MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
		if (i == checkpoints.end()) return true;
		return hash == i->second;
	}

	int GetTotalBlocksEstimate()
	{
		MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

		if (checkpoints.empty())
			return 0;
		return checkpoints.rbegin()->first;
	}

	CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
	{
		MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

		BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
		{
			const uint256& hash = i.second;
			std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
			if (t != mapBlockIndex.end())
				return t->second;
		}
		return NULL;
	}

	// Automatically select a suitable sync-checkpoint 
	const CBlockIndex* AutoSelectSyncCheckpoint()
	{
		const CBlockIndex *pindex = pindexBest;
		// Search backward for a block within max span and maturity window
		while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
			pindex = pindex->pprev;
		return pindex;
	}

	// Check against synchronized checkpoint
	bool CheckSync(int nHeight)
	{
		const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
		if (nHeight <= pindexSync->nHeight) {
			return false;
		}
		return true;
	}
}
