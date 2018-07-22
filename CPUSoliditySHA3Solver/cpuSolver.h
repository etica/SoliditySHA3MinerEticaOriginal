#include <cassert>
#include <atomic>
#include <string>
#include <random>
#include <vector>
#include "types.h"
#include "uint256/arith_uint256.h"

#pragma managed(push, off)

#ifdef _M_CEE
#	undef _M_CEE
#	include <mutex>
#	include <thread>
#	define _M_CEE 001
#else
#	include <mutex>
#	include <thread>
#endif

#pragma managed(pop)

namespace CPUSolver
{
	class cpuSolver
	{
	public:
		typedef void(*MessageCallback)(int, const char*, const char*);
		typedef void(*SolutionCallback)(const char*, const char*, const char*, const char*, const char*, const char*, bool);

		bool m_SubmitStale;

	private:
		static std::atomic<bool> m_pause;

		MessageCallback m_messageCallback;
		SolutionCallback m_solutionCallback;

		std::atomic<byte32_t> b_target;
		arith_uint256 m_target;
		arith_uint256 m_difficulty;
		arith_uint256 m_maxDifficulty;
		arith_uint256 m_customDifficulty;

		std::string s_kingAddress;
		std::string s_address;
		std::string s_challenge;
		std::string s_target;
		std::string s_difficulty;
		std::string s_customDifficulty;

		address_t m_address;
		byte32_t m_solutionTemplate;
		prefix_t m_prefix; // challenge32 + address20

		uint32_t m_miningThreadCount;
		uint32_t* m_miningThreadAffinities;
		bool* m_isThreadMining;

		uint64_t* m_threadHashes;
		std::chrono::steady_clock::time_point m_hashStartTime;
		std::atomic<uint64_t> m_solutionHashCount;
		std::atomic<std::chrono::steady_clock::time_point> m_solutionHashStartTime;

	public:
		static uint32_t getLogicalProcessorsCount();
		static std::string getSolutionTemplate(std::string kingAddress = "");

		cpuSolver(std::string const maxDifficulty, std::string const threads, std::string solutionTemplate, std::string kingAddress) noexcept;
		~cpuSolver() noexcept;

		void setMessageCallback(MessageCallback messageCallback);
		void setSolutionCallback(SolutionCallback solutionCallback);

		bool isMining();
		bool isPaused();

		void updatePrefix(std::string const prefix);
		void updateTarget(std::string const target);
		void updateDifficulty(std::string const difficulty);
		void setCustomDifficulty(uint32_t customDifficulty);

		uint64_t getTotalHashRate();
		uint64_t getHashRateByThreadID(uint32_t const threadID);

		void startFinding();
		void stopFinding();
		void pauseFinding(bool pauseFinding);

	private:
		void onMessage(int threadID, const char* type, const char* message);
		void onMessage(int threadID, std::string type, std::string message);
		void onSolution(byte32_t const solution, byte32_t const digest, std::string challenge);
		bool setCurrentThreadAffinity(uint32_t const affinityMask);
		void findSolution(uint32_t const threadID, uint32_t const affinityMask);
	};
}