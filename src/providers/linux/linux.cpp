#include <iostream>

#include <common/state/CurrentState.hpp>
#include <common/trace/Event.hpp>

extern "C" {

void onEvent(tibee::common::CurrentState& state, tibee::common::Event& event)
{
    auto fields = event.getFields();

    if (fields) {
        std::cout << fields->toString() << std::endl;
    }
}

}