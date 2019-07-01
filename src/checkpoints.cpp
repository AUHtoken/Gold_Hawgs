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
		(8541, uint256("0x41ce3b0d368f2c4b31ed4b56fcdf4b0478e68990e1cc721a1af18109f07cfbdf"))		

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
