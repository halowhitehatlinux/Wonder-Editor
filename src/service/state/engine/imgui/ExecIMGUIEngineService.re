open Wonderjs;

let createEmptyExecFunc = () => (. _, _, state) => state;

let getExecFunc = ExecIMGUIAPI.getExecFunc;

let unsafeGetExecFunc = ExecIMGUIAPI.unsafeGetExecFunc;

let hasExecFuncData = ExecIMGUIAPI.hasExecFuncData;

let addExecFuncData = ExecIMGUIAPI.addExecFuncData;

let removeExecFuncData = ExecIMGUIAPI.removeExecFuncData;

let updateExecFuncData =
    (
      oldExecFuncName,
      newExecFuncName,
      customData,
      zIndex,
      func: ExecIMGUIType.execFunc,
      state,
    ) =>
  state
  |> removeExecFuncData(oldExecFuncName)
  |> addExecFuncData(newExecFuncName, customData, zIndex, func);

let clearExecFuncDataArr = ExecIMGUIAPI.clearExecFuncDataArr;

let getCustomData = ExecIMGUIMainService.getCustomData;

let unsafeGetCustomData = ExecIMGUIMainService.unsafeGetCustomData;

let getZIndex = ExecIMGUIMainService.getZIndex;

let unsafeGetZIndex = ExecIMGUIMainService.unsafeGetZIndex;