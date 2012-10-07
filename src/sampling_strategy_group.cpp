#include "sampling_strategy_group.h"
#include "sampling_strategy.h"

SamplingStrategyGroup::~SamplingStrategyGroup() {
    clearStrategies();
}

void SamplingStrategyGroup::addStrategy(SamplingStrategy* strategy) {
    strategy->sampler = &sampler;
    strategies_.push_back(strategy);
}

void SamplingStrategyGroup::clearStrategies() {
    for (SamplingStrategy* strategy : strategies_) {
        delete strategy;
    }
    strategies_.erase(strategies_.begin(), strategies_.end());
}
