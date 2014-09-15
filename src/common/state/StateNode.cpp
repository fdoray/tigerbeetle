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
#include <common/state/StateNode.hpp>
#include <common/state/NullStateValue.hpp>
#include <common/state/Sint32StateValue.hpp>
#include <common/state/Uint32StateValue.hpp>
#include <common/state/Sint64StateValue.hpp>
#include <common/state/Uint64StateValue.hpp>
#include <common/state/Float32StateValue.hpp>
#include <common/state/QuarkStateValue.hpp>
#include <common/state/CurrentState.hpp>

namespace tibee
{
namespace common
{

StateNode::StateNode(state_node_id_t id, CurrentState* currentState,
                     timestamp_t beginTs, StateNode* parent,
                     Quark quark) :
    _id {id},
    _beginTs {beginTs},
    _currentState {currentState},
    _parent {parent},
    _quark {quark}
{
    // initial state value (null)
    _stateValue = AbstractStateValue::UP {new NullStateValue};
}

StateNode::~StateNode()
{
}

void StateNode::getPath(std::vector<Quark>* path) const
{
    if (_parent == nullptr)
    {
        path->clear();
        return;
    }
    _parent->getPath(path);
    path->push_back(_quark);
}

const AbstractStateValue& StateNode::getValue() const
{
    if (_stateValue) {
        return *_stateValue;
    } else {
        return _currentState->getNull();
    }
}

StateNode& StateNode::operator[](Quark quark)
{
    // child exists?
    if (this->childExists(quark.get())) {
        return *_children[quark.get()];
    }

    auto newNode = _currentState->buildStateNode(this, quark);

    _children[quark.get()] = std::move(newNode);

    return *_children[quark.get()];
}

StateNode& StateNode::operator[](const std::string& key)
{
    // get quark for this subpath
    auto quark = _currentState->getQuark(key);

    // delegate
    return this->operator[](quark);
}

StateNode& StateNode::operator[](const char* key)
{
    return this->operator[](std::string(key));
}

StateNode& StateNode::operator[](std::int64_t key)
{
    return this->operator[](std::to_string(key));
}

StateNode& StateNode::operator[](std::uint64_t key)
{
    return this->operator[](std::to_string(key));
}

StateNode& StateNode::operator[](std::int32_t key)
{
    return this->operator[](std::to_string(key));
}

StateNode& StateNode::operator[](std::uint32_t key)
{
    return this->operator[](std::to_string(key));
}

StateNode& StateNode::operator[](float key)
{
    return this->operator[](std::to_string(key));
}

StateNode& StateNode::operator[](const SintEventValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const UintEventValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const StringEventValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const AbstractEventValue& value)
{
    if (value.isSint()) {
        return this->operator[](value.asSint());
    } else if (value.isUint()) {
        return this->operator[](value.asUint());
    } else if (value.isString()) {
        return this->operator[](value.asString());
    } else {
        return this->operator[]("");
    }
}

StateNode& StateNode::operator[](const Sint32StateValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const Sint64StateValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const Uint32StateValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const Uint64StateValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const Float32StateValue& value)
{
    return this->operator[](value.getValue());
}

StateNode& StateNode::operator[](const QuarkStateValue& value)
{
    // get string value for this string value quark
    const auto& str = _currentState->getString(value.getValue());

    // get subpath quark for this string
    auto subpathQuark = _currentState->getQuark(str);

    // delegate
    return this->operator[](subpathQuark);
}

StateNode::Iterator StateNode::begin()
{
    return StateNode::Iterator {
        _children.begin(),
        _children.end()
    };
}

StateNode::Iterator StateNode::end()
{
    return StateNode::Iterator {
        _children.end(),
        _children.end()
    };
}

bool StateNode::hasChild(Quark quark) const
{
    // does the child event exists?
    if (this->childExists(quark.get())) {
        // get it and check if it's not marked as removed
        return !_children.at(quark.get())->isNull();
    }

    return false;
}

bool StateNode::hasChild(const std::string& key) const
{
    // get quark for this subpath
    auto quark = _currentState->getQuark(key);

    // delegate
    return this->hasChild(quark);
}

bool StateNode::hasChild(const char* key) const
{
    return this->hasChild(std::string(key));
}

bool StateNode::hasChild(std::int64_t key) const
{
    return this->hasChild(std::to_string(key));
}

bool StateNode::hasChild(std::uint64_t key) const
{
    return this->hasChild(std::to_string(key));
}

bool StateNode::hasChild(const SintEventValue& key) const
{
    return this->hasChild(key.getValue());
}

bool StateNode::hasChild(const UintEventValue& key) const
{
    return this->hasChild(key.getValue());
}

bool StateNode::hasChild(const StringEventValue& key) const
{
    return this->hasChild(key.getValue());
}

bool StateNode::hasChild(const AbstractEventValue& key) const
{
    if (key.isSint()) {
        return this->hasChild(key.asSint());
    } else if (key.isUint()) {
        return this->hasChild(key.asUint());
    } else if (key.isString()) {
        return this->hasChild(key.asString());
    }

    return false;
}

std::size_t StateNode::getChildrenCount() const
{
    std::size_t count = 0;

    // TODO: cache this count in an attribute of this node
    for (const auto& quarkNodePair : _children) {
        if (*quarkNodePair.second) {
            count++;
        }
    }

    return count;
}

std::size_t StateNode::getAllChildrenCount() const
{
    return _children.size();
}

StateNode& StateNode::operator=(const AbstractStateValue& value)
{
    // TODO: use a LUT here
    if (value.isSint32()) {
        return (*this = value.asSint32Value());
    } else if (value.isUint32()) {
        return (*this = value.asUint32Value());
    } else if (value.isSint64()) {
        return (*this = value.asSint64Value());
    } else if (value.isUint64()) {
        return (*this = value.asUint64Value());
    } else if (value.isFloat32()) {
        return (*this = value.asFloat32Value());
    } else if (value.isQuark()) {
        return (*this = value.asQuarkValue());
    }

    return (*this = _currentState->getNull());
}

StateNode& StateNode::operator=(const StateNode& node)
{
    return (*this = node.getValue());
}

StateNode& StateNode::operator=(Quark quark)
{
    return (*this = QuarkStateValue {quark});
}

StateNode& StateNode::operator=(const std::string& value)
{
    // get quark for this string value
    auto quark = _currentState->getQuark(value);

    // delegate
    return (*this = quark);
}

StateNode& StateNode::operator=(const char* value)
{
    return (*this = std::string(value));
}

StateNode& StateNode::operator=(std::int32_t value)
{
    return (*this = Sint32StateValue {value});
}

StateNode& StateNode::operator=(std::uint32_t value)
{
    return (*this = Uint32StateValue {value});
}

StateNode& StateNode::operator=(std::int64_t value)
{
    return (*this = Sint64StateValue {value});
}

StateNode& StateNode::operator=(std::uint64_t value)
{
    return (*this = Uint64StateValue {value});
}

StateNode& StateNode::operator=(float value)
{
    return (*this = Float32StateValue {value});
}

StateNode& StateNode::operator=(const SintEventValue& value)
{
    return (*this = Sint64StateValue {value.getValue()});
}

StateNode& StateNode::operator=(const UintEventValue& value)
{
    return (*this = Uint64StateValue {value.getValue()});
}

StateNode& StateNode::operator=(const FloatEventValue& value)
{
    return (*this = Float32StateValue {
        static_cast<float>(value.getValue())
    });
}

StateNode& StateNode::operator=(const StringEventValue& value)
{
    return (*this = value.getValue());
}

StateNode& StateNode::operator=(const AbstractEventValue& value)
{
    if (value.isSint()) {
        return this->operator=(value.asSint());
    } else if (value.isUint()) {
        return this->operator=(value.asUint());
    } else if (value.isString()) {
        return (*this = value.asString());
    } else if (value.isFloat()) {
        return (*this = static_cast<float>(value.asFloat()));
    }

    return *this;
}

StateNode& StateNode::setNull()
{
    return (*this = _currentState->getNull());
}

StateNode& StateNode::setNullRecursive()
{
    // nullify myself
    this->setNull();

    // nullify my children
    for (const auto& quarkNodePair : _children) {
        quarkNodePair.second->setNull();
    }

    return *this;
}

StateNode& StateNode::operator++()
{
    return (*this += 1);
}

StateNode& StateNode::operator--()
{
    return (*this -= 1);
}

StateNode& StateNode::operator+=(std::int64_t inc)
{
    if (!_stateValue) {
        return *this;
    }

    if (_stateValue->isSint32()) {
        auto curValue = _stateValue->asSint32();

        return this->operator=(static_cast<std::int32_t>(curValue + inc));
    } else if (_stateValue->isUint32()) {
        auto curValue = _stateValue->asUint32();

        return this->operator=(static_cast<std::uint32_t>(curValue + inc));
    } else if (_stateValue->isSint64()) {
        auto curValue = _stateValue->asSint64();

        return this->operator=(static_cast<std::int64_t>(curValue + inc));
    } else if (_stateValue->isUint64()) {
        auto curValue = _stateValue->asUint64();

        return this->operator=(static_cast<std::uint64_t>(curValue + inc));
    }

    return *this;
}

StateNode& StateNode::operator-=(std::int64_t dec)
{
    if (!_stateValue) {
        return *this;
    }

    if (_stateValue->isSint32()) {
        auto curValue = _stateValue->asSint32();

        return this->operator=(static_cast<std::int32_t>(curValue - dec));
    } else if (_stateValue->isUint32()) {
        auto curValue = _stateValue->asUint32();

        return this->operator=(static_cast<std::uint32_t>(curValue - dec));
    } else if (_stateValue->isSint64()) {
        auto curValue = _stateValue->asSint64();

        return this->operator=(static_cast<std::int64_t>(curValue - dec));
    } else if (_stateValue->isUint64()) {
        auto curValue = _stateValue->asUint64();

        return this->operator=(static_cast<std::uint64_t>(curValue - dec));
    }

    return *this;
}

void StateNode::acceptUpdate(AbstractStateNodeVisitor& visitor,
                             quark_t quark)
{
    // me first
    visitor.visitUpdateEnter(quark, *this);

    // then my children
    for (auto& quarkNodePair : _children) {
        quarkNodePair.second->acceptUpdate(visitor, quarkNodePair.first);
    }

    // leaving
    visitor.visitUpdateLeave(quark, *this);
}

void StateNode::acceptRead(AbstractStateNodeVisitor& visitor,
                           quark_t quark) const
{
    // me first
    visitor.visitReadEnter(quark, *this);

    // then my children
    for (auto& quarkNodePair : _children) {
        quarkNodePair.second->acceptRead(visitor, quarkNodePair.first);
    }

    // leaving
    visitor.visitReadLeave(quark, *this);
}

timestamp_t StateNode::getCurrentSinkTimestamp()
{
    return _currentState->getCurrentTimestamp();
}

void StateNode::onStateChange(const AbstractStateValue& newValue) const {
    _currentState->onStateChange(*this, newValue);
}

}
}
