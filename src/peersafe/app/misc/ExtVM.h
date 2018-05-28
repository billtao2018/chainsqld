#ifndef CHAINSQL_APP_MISC_EXTVM_H_INCLUDED
#define CHAINSQL_APP_MISC_EXTVM_H_INCLUDED

#include <peersafe/app/misc/SleOps.h>
#include <ripple/app/tx/impl/ApplyContext.h>
#include <peersafe/vm/ExtVMFace.h>
#include <peersafe/app/misc/TypeTransform.h>

#include <functional>
#include <map>

namespace ripple
{

class EnvEnfoImpl : public EnvInfo
{
public:
    EnvEnfoImpl(ApplyContext& ctx);
private:
    ApplyContext &ctx_;
};

class ExtVM : public ExtVMFace
{
public:    
    ExtVM(SleOps& _s, EnvInfo const& _envInfo, AccountID const& _myAddress,
        AccountID const& _caller, AccountID const& _origin, uint256 _value, uint256 _gasPrice, bytesConstRef _data,
        bytesConstRef _code, uint256 const& _codeHash, int32_t _depth, bool _isCreate,  bool _staticCall)
      : ExtVMFace(_envInfo, toEvmC(_myAddress), toEvmC(_caller), toEvmC(_origin), toEvmC(_value), toEvmC(_gasPrice), _data, _code.toBytes(), toEvmC(_codeHash), _depth, _isCreate, _staticCall),
        m_s(_s)
    {
        // Contract: processing account must exist. In case of CALL, the ExtVM
        // is created only if an account has code (so exist). In case of CREATE
        // the account must be created first.
        
        //assert(m_s.addressInUse(_myAddress));
    }

	/// Read storage location.
    virtual evmc_uint256be store(evmc_uint256be const& key) override final;

	/// Write a value in storage.
	virtual void setStore(evmc_uint256be const& key, evmc_uint256be const& value) override final;

    /// Read address's balance.
    virtual evmc_uint256be balance(evmc_address const&) { return evmc_uint256be(); }

    /// Read address's code.
    virtual bytes const& codeAt(evmc_address const&) { return NullBytes; }

    /// @returns the size of the code in bytes at the given address.
    virtual size_t codeSizeAt(evmc_address const&) { return 0; }

    /// Does the account exist?
    virtual bool exists(evmc_address const&) { return false; }

    /// Suicide the associated contract and give proceeds to the given address.
    virtual void suicide(evmc_address const&) { }

    /// Create a new (contract) account.
    virtual CreateResult create(evmc_uint256be const&, int64_t const&,
        bytesConstRef const&, Instruction, evmc_uint256be const&) = 0;

    /// Make a new message call.
    virtual CallResult call(CallParameters&) = 0;

    /// Revert any changes made (by any of the other calls).
    virtual void log(evmc_uint256be const* topics, size_t numTopics, bytesConstRef const& _data) {  }

    /// Hash of a block if within the last 256 blocks, or h256() otherwise.
    virtual evmc_uint256be blockHash(int64_t  const&_number) = 0;

    SleOps const& state() const { return m_s; }

private:
	SleOps& m_s;  ///< A reference to the sleOp	
};

}

#endif