#ifndef _SAMPLING_STRATEGY_GROUP_H_
#define _SAMPLING_STRATEGY_GROUP_H_

#include <vector>
#include "uv_sampler.h"

class SamplingStrategy;

/**
 * A class for managing a group of sampling strategies,
 * all of which are used with the same UVSampler
 */
class SamplingStrategyGroup {

    typedef std::vector<SamplingStrategy* > StrategyContainer;

public:
    /**
     * Create a new empty SamplingStrategyGroup, with a sampler
     * that is configured with @a numSamples
     */
    SamplingStrategyGroup(int numSamples) : sampler(numSamples) { }
    ~SamplingStrategyGroup();

    /**
     * Add a sampling strategy to the group
     * ownership is transferred to the group.
     */
    void addStrategy(SamplingStrategy* strategy);

    /**
     * Clear the group of strategies
     */
    void clearStrategies();
    
    UVSampler sampler; ///< Sampler common to all SamplingStrategy objects managed

    typedef StrategyContainer::const_iterator const_iterator;
    const_iterator begin() const { return strategies_.begin(); }
    const_iterator end() const { return strategies_.end(); }

    std::size_t size() const { return strategies_.size(); }

private:
    StrategyContainer strategies_;

};

#endif // _SAMPLING_STRATEGY_GROUP_H_
