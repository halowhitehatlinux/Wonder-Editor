'use strict';

import * as MainEditorSceneEdit$WonderEditor      from "../edit/MainEditorSceneEdit.js";
import * as MainEditorPrimitiveOper$WonderEditor  from "../operator/MainEditorPrimitiveOper.js";
import * as MainEditorGameObjectOper$WonderEditor from "../operator/MainEditorGameObjectOper.js";

function unsafeGetScene(param) {
  return MainEditorSceneEdit$WonderEditor.unsafeGetScene(param[0]);
}

function unsafeGetCurrentGameObject(param) {
  return MainEditorSceneEdit$WonderEditor.unsafeGetCurrentGameObject(param[0]);
}

function getCurrentGameObject(param) {
  return MainEditorSceneEdit$WonderEditor.getCurrentGameObject(param[0]);
}

function hasCurrentGameObject(param) {
  return MainEditorSceneEdit$WonderEditor.hasCurrentGameObject(param[0]);
}

function setCurrentGameObject(gameObject, param) {
  return /* tuple */[
          MainEditorSceneEdit$WonderEditor.setCurrentGameObject(gameObject, param[0]),
          param[1]
        ];
}

function clearCurrentGameObject(param) {
  return /* tuple */[
          MainEditorSceneEdit$WonderEditor.clearCurrentGameObject(param[0]),
          param[1]
        ];
}

function addBoxGameObject(stateTuple) {
  var match = MainEditorPrimitiveOper$WonderEditor.createBox(stateTuple[1]);
  var box = match[1];
  return /* tuple */[
          box,
          /* tuple */[
            stateTuple[0],
            MainEditorGameObjectOper$WonderEditor.addChild(unsafeGetScene(stateTuple), box, MainEditorGameObjectOper$WonderEditor.initGameObject(box, match[0]))
          ]
        ];
}

function disposeCurrentGameObject(gameObject, param) {
  return /* tuple */[
          param[0],
          MainEditorGameObjectOper$WonderEditor.disposeGameObject(gameObject, param[1])
        ];
}

function disposeGameObjectChildren(gameObject, param) {
  var engineState = param[1];
  return /* tuple */[
          param[0],
          MainEditorGameObjectOper$WonderEditor.getChildren(gameObject, engineState).reduce((function (engineState, gameObject) {
                  return MainEditorGameObjectOper$WonderEditor.disposeGameObject(gameObject, engineState);
                }), engineState)
        ];
}

export {
  unsafeGetScene             ,
  unsafeGetCurrentGameObject ,
  getCurrentGameObject       ,
  hasCurrentGameObject       ,
  setCurrentGameObject       ,
  clearCurrentGameObject     ,
  addBoxGameObject           ,
  disposeCurrentGameObject   ,
  disposeGameObjectChildren  ,
  
}
/* MainEditorSceneEdit-WonderEditor Not a pure module */