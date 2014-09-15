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
#include <common/state/CurrentState.hpp>
#include <common/state/StateChangeNotification.hpp>
#include <common/state/StateNode.hpp>

namespace tibee
{
namespace common
{

CurrentState::CurrentState() :
    _ts {0},
    _nextNodeId {0},
    _stateChangeSink {nullptr},
    _stringDb {new QuarkDatabase<std::string>()}
{
  _null = NullStateValue::UP {new NullStateValue};

  // create root node
  _root = this->buildStateNode(nullptr, Quark(0));
}

CurrentState::~CurrentState()
{
}

Quark CurrentState::getQuark(const std::string& subpath) const
{
    return _stringDb->Insert(subpath);
}

const std::string& CurrentState::getString(Quark quark) const
{
    return _stringDb->ValueOf(quark);
}

StateNode& CurrentState::getRoot()
{
    return *_root;
}

const NullStateValue& CurrentState::getNull() const
{
    return *_null;
}

StateNode::UP CurrentState::buildStateNode(StateNode* parent, Quark quark)
{
    // build node
    StateNode::UP node {new StateNode {_nextNodeId, this, 0, parent, quark}};

    // update next node ID
    _nextNodeId++;

    return node;
}

void CurrentState::onStateChange(const StateNode& stateNode,
                                 const AbstractStateValue& newValue) {
    if (_stateChangeSink == nullptr)
        return; 
    StateChangeNotification notification(*this, stateNode, newValue);
    _stateChangeSink->Receive(notification);
}

}
}
