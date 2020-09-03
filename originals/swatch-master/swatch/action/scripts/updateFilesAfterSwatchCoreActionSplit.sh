#!/bin/bash

FILES=$@

MIGRATED_CLASSES=ActionableObject
MIGRATED_CLASSES+=" ActionableStatus"
MIGRATED_CLASSES+=" ActionableSystem"
MIGRATED_CLASSES+=" BusyGuard"
MIGRATED_CLASSES+=" Command"
MIGRATED_CLASSES+=" CommandSequence"
MIGRATED_CLASSES+=" CommandVec"
MIGRATED_CLASSES+=" Functionoid"
MIGRATED_CLASSES+=" GateKeeper"
MIGRATED_CLASSES+=" GateKeeperView"
MIGRATED_CLASSES+=" MaskableObject"
MIGRATED_CLASSES+=" MonitoringSetting"
MIGRATED_CLASSES+=" ObjectFunctionoid"
MIGRATED_CLASSES+=" StateMachine"
MIGRATED_CLASSES+=" SystemBusyGuard"
MIGRATED_CLASSES+=" SystemFunctionoid"
MIGRATED_CLASSES+=" SystemStateMachine"
MIGRATED_CLASSES+=" ThreadPool" 

MIGRATED_HEADERS="${MIGRATED_CLASSES} DefaultCommandParRules"

MIGRATED_CLASSES+=" CommandSnapshot"
MIGRATED_CLASSES+=" CommandSequenceSnapshot"
MIGRATED_CLASSES+=" CommandVecSnapshot"
MIGRATED_CLASSES+=" Transition"
MIGRATED_CLASSES+=" TransitionSnapshot"
MIGRATED_CLASSES+=" SystemTransition"
MIGRATED_CLASSES+=" SystemTransitionSnapshot"

MIGRATED_CLASSES+=" ActionableObjectIsBusy"
MIGRATED_CLASSES+=" CommandAlreadyExistsInActionableObject"
MIGRATED_CLASSES+=" CommandNotFoundInActionableObject"
MIGRATED_CLASSES+=" CommandSequenceAlreadyExistsInActionableObject"
MIGRATED_CLASSES+=" CommandSequenceNotFoundInActionableObject"
MIGRATED_CLASSES+=" StateMachineAlreadyExistsInActionableObject"
MIGRATED_CLASSES+=" StateMachineNotFoundInActionableObject"
MIGRATED_CLASSES+=" IncorrectActionableGuard"
MIGRATED_CLASSES+=" ActionableSystemIsBusy"
MIGRATED_CLASSES+=" WrongBusyGuard"
MIGRATED_CLASSES+=" CommandConstraintRegistrationFailed"
MIGRATED_CLASSES+=" CommandParameterCheckFailed"
MIGRATED_CLASSES+=" CommandParameterRegistrationFailed"
MIGRATED_CLASSES+=" OutOfRange"
MIGRATED_CLASSES+=" ParameterNotFound"
MIGRATED_CLASSES+=" InvalidResource"
MIGRATED_CLASSES+=" ContextWithIdAlreadyExists"
MIGRATED_CLASSES+=" MSettingWithGivenIdAlreadyExistsInContext"
MIGRATED_CLASSES+=" NonExistentKey"
MIGRATED_CLASSES+=" ParameterWithGivenIdAlreadyExistsInContext"
MIGRATED_CLASSES+=" ResourceInWrongState"
MIGRATED_CLASSES+=" ResourceInWrongStateMachine"
MIGRATED_CLASSES+=" StateAlreadyDefined"
MIGRATED_CLASSES+=" StateNotDefined"
MIGRATED_CLASSES+=" TransitionAlreadyDefined"
MIGRATED_CLASSES+=" TransitionNotDefined"
MIGRATED_CLASSES+=" InvalidSystemTransition"
MIGRATED_CLASSES+=" OperationOnStoppedThreadPool"


set +x
echo MIGRATED_HEADERS=${MIGRATED_HEADERS}
echo MIGRATED_CLASSES=${MIGRATED_CLASSES}


for FilePath in ${FILES} ; do 

  echo "Editing ${FilePath} ..."

  for ClassName in ${MIGRATED_CLASSES}; do
    sed -i -re 's|^(.*#include\s+"swatch/)core(/'${ClassName}'.hpp".*)$|\1action\2|g' ${FilePath}
    sed -i -re 's|^(.*#include\s+"swatch/)core(/'${ClassName}'.hxx".*)$|\1action\2|g' ${FilePath}

    sed -i -re 's|core::'${ClassName}'|action::'${ClassName}'|g' ${FilePath}
  done

done

