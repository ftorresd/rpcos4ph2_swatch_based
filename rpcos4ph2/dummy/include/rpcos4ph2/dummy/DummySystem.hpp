/*
 * File:   DummySystem.hpp
 * Author: tom
 *
 * Created on 18 September 2015, 14:05
 */

#ifndef _RPCOS4PH2_DUMMY_DUMMYSYSTEM_HPP__
#define _RPCOS4PH2_DUMMY_DUMMYSYSTEM_HPP__

#include "swatch/system/System.hpp"
#include "swatch/action/SystemStateMachine.hpp"


namespace rpcos4ph2
{
    // namespace action {
    // class SystemTransitionSnapshot;
    // }

    namespace dummy
    {

        class DummySystem : public swatch::system::System
        {
        public:
            DummySystem(const swatch::core::AbstractStub &aStub);
            ~DummySystem();

        private:
            static std::string analyseSourceOfWarning(const swatch::action::SystemTransitionSnapshot &);
            static std::string analyseSourceOfError(const swatch::action::SystemTransitionSnapshot &);
        };

    } // namespace dummy
} // namespace rpcos4ph2

#endif /* _RPCOS4PH2_DUMMY_DUMMYSYSTEM_HPP__ */
