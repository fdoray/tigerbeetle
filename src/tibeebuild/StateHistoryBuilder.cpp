/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <memory>
#include <boost/filesystem/path.hpp>

#include <common/trace/EventValueType.hpp>
#include <common/trace/AbstractEventValue.hpp>
#include <common/stateprov/AbstractStateProvider.hpp>
#include <common/stateprov/DynamicLibraryStateProvider.hpp>
#include <common/stateprov/PythonStateProvider.hpp>
#include <common/stateprov/StateProviderConfig.hpp>
#include <common/ex/WrongStateProvider.hpp>
#include "AbstractCacheBuilder.hpp"
#include "StateHistoryBuilder.hpp"
#include "ex/UnknownStateProviderType.hpp"
#include "ex/StateProviderNotFound.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{

StateHistoryBuilder::StateHistoryBuilder(const bfs::path& dbDir,
                                         const std::vector<common::StateProviderConfig>& providers) :
    AbstractCacheBuilder {dbDir},
    _providersConfigs {providers}
{
    for (const auto& providerConfig : _providersConfigs) {
        auto providerPath = bfs::path {providerConfig.getName()};

        // make sure the file exists
        if (!bfs::exists(providerPath)) {
            throw ex::StateProviderNotFound {providerConfig.getName()};
        }

        // only files are supported for the moment
        if (bfs::is_directory(providerPath)) {
            throw common::ex::WrongStateProvider {
                "provider is a directory",
                providerConfig.getName()
            };
        }

        // known providers are right here for the moment
        auto extension = providerPath.extension();

        common::AbstractStateProvider::UP stateProvider;

        if (extension == ".so" || extension == ".dll" || extension == ".dylib") {
            stateProvider = common::AbstractStateProvider::UP {
                new common::DynamicLibraryStateProvider {providerPath, providerConfig}
            };
        } else if (extension == ".py") {
            stateProvider = common::AbstractStateProvider::UP {
                new common::PythonStateProvider {providerPath, providerConfig}
            };
        } else {
            throw ex::UnknownStateProviderType {providerConfig.getName()};
        }

        _providers.push_back(std::move(stateProvider));
    }
}

StateHistoryBuilder::~StateHistoryBuilder()
{
}

bool StateHistoryBuilder::onStartImpl(const common::TraceSet* traceSet)
{
    _currentState->SetStateChangeSink(_sink);

    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onInit(*_currentState, traceSet);
    }

    return true;
}

void StateHistoryBuilder::onEventImpl(const common::Event& event)
{
    // update current timestamp
    _currentState->setCurrentTimestamp(event.getTimestamp());

    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onEvent(*_currentState, event);
    }
}

bool StateHistoryBuilder::onStopImpl()
{
    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onFini(*_currentState);
    }

    return true;
}

std::size_t StateHistoryBuilder::getStateChanges() const
{
    // TODO
    return 0;
}

}
