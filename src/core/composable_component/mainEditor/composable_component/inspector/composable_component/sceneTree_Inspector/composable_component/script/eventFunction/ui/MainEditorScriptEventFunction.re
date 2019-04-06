type state = {
  currentScript: Wonderjs.ScriptType.script,
  isShowScriptEventFunctionGroupForAdd: bool,
  isShowScriptEventFunctionGroupForChange: bool,
  lastScriptEventFunctionNodeIdForAdd: option(int),
  lastScriptEventFunctionNodeIdForChange: option(int),
  unUsedScriptEventFunctionNodeIds: array(NodeAssetType.nodeId),
};

type action =
  | ChangeScriptEventFunctionForAdd(
      NodeAssetType.nodeId,
      array(NodeAssetType.nodeId),
    )
  | ChangeScriptEventFunctionForChange(
      NodeAssetType.nodeId,
      array(NodeAssetType.nodeId),
    )
  | ShowScriptEventFunctionGroupForAdd(
      NodeAssetType.nodeId,
      array(NodeAssetType.nodeId),
    )
  | ShowScriptEventFunctionGroupForChange(
      NodeAssetType.nodeId,
      array(NodeAssetType.nodeId),
    )
  | HideScriptEventFunctionGroupForAdd
  | HideScriptEventFunctionGroupForChange;

module Method = {
  let _changeScriptEventFunction =
      (
        currentScript,
        currentScriptEventFunctionNodeIdOpt,
        targetScriptEventFunctionNodeId,
        (editorState, engineState),
      ) => {
    WonderLog.Contract.requireCheck(
      () =>
        WonderLog.(
          Contract.(
            Operators.(
              test(
                Log.buildAssertMessage(
                  ~expect={j|targetScriptEventFunctionNodeId not be used|j},
                  ~actual={j|be used|j},
                ),
                () => {
                  let (name, _) =
                    ScriptEventFunctionNodeAssetEditorService.getNameAndData(
                      targetScriptEventFunctionNodeId,
                      editorState,
                    );

                  ScriptEngineService.hasScriptEventFunctionData(
                    currentScript,
                    name,
                    engineState,
                  )
                  |> assertFalse;
                },
              )
            )
          )
        ),
      StateEditorService.getStateIsDebug(),
    );

    let (targetName, targetEventFunction) =
      ScriptEventFunctionNodeAssetEditorService.getNameAndData(
        targetScriptEventFunctionNodeId,
        editorState,
      );

    switch (currentScriptEventFunctionNodeIdOpt) {
    | None =>
      ScriptEngineService.addScriptEventFunctionData(
        currentScript,
        targetName,
        targetEventFunction,
        engineState,
      )
    | Some(currentScriptEventFunctionNodeId) =>
      let (sourceName, _) =
        ScriptEventFunctionNodeAssetEditorService.getNameAndData(
          currentScriptEventFunctionNodeId,
          editorState,
        );

      ScriptEngineService.replaceScriptEventFunctionData(
        currentScript,
        (sourceName, targetName),
        targetEventFunction,
        engineState,
      );
    };
  };

  let _getUnUsedScriptEventFunctionNodes =
      (script, (editorState, engineState)) => {
    let allScriptEventFunctionNodes =
      ScriptEventFunctionNodeAssetEditorService.findAllScriptEventFunctionNodes(
        editorState,
      );

    let scriptAllEventFunctionEntries =
      ScriptEngineService.getScriptAllEventFunctionEntries(
        script,
        engineState,
      );

    let allEventFunctionNames =
      scriptAllEventFunctionEntries
      |> Js.Array.map(((eventFunctionName, _)) => eventFunctionName);

    ArrayService.excludeWithFunc(
      scriptAllEventFunctionEntries,
      (scriptAllEventFunctionEntries, scriptEventFunctionNode) =>
        allEventFunctionNames
        |> Js.Array.includes(
             ScriptEventFunctionNodeAssetService.getNodeName(
               scriptEventFunctionNode,
             ),
           ),
      allScriptEventFunctionNodes,
    );
  };

  let _getUnUsedScriptEventFunctionNodeIds =
      (script, (editorState, engineState)) =>
    _getUnUsedScriptEventFunctionNodes(script, (editorState, engineState))
    |> Js.Array.map(node => NodeAssetService.getNodeId(~node));

  let _sendShowScriptEventFunctionGroupForChange =
      (
        currentScript,
        scriptEventFunctionNodeId,
        send,
        (editorState, engineState),
      ) =>
    send(
      ShowScriptEventFunctionGroupForChange(
        scriptEventFunctionNodeId,
        _getUnUsedScriptEventFunctionNodeIds(
          currentScript,
          (editorState, engineState),
        ),
      ),
    );

  let _removeScriptEventFunction = (script, attributeName, dispatchFunc) => {
    ScriptEngineService.removeScriptEventFunctionData(script, attributeName)
    |> StateLogicService.getAndSetEngineState;

    dispatchFunc(AppStore.UpdateAction(Update([|UpdateStore.Inspector|])))
    |> ignore;
  };

  let renderScriptAllEventFunctions =
      (
        languageType,
        dispatchFunc,
        {state, send}: ReasonReact.self('a, 'b, 'c),
      ) => {
    let {currentScript} = state;

    let unUsedScriptEventFunctionNodeIds =
      _getUnUsedScriptEventFunctionNodeIds(currentScript)
      |> StateLogicService.getStateToGetData;

    ScriptEngineService.getScriptAllEventFunctionEntries(currentScript)
    |> StateLogicService.getEngineStateToGetData
    |> Js.Array.map(((name, attribute)) => {
         let scriptEventFunctionNodeId =
           OperateTreeAssetLogicService.findNodeIdByName(name)
           |> StateLogicService.getStateToGetData
           |> OptionService.unsafeGet;

         <div key={DomHelper.getRandomKey()}>
           <SelectAssetGroupBar
             headerText="Script Event Function"
             headerTitle={
               LanguageUtils.getInspectorLanguageDataByType(
                 "script-use-scriptEventFunction-describe",
                 languageType,
               )
             }
             assetText=name
             selectAssetFunc={
               send =>
                 _sendShowScriptEventFunctionGroupForChange(
                   currentScript,
                   scriptEventFunctionNodeId,
                   send,
                 )
                 |> StateLogicService.getStateToGetData
             }
             sendFunc=send
           />
           <button
             className="scriptEventFunction-remove"
             onClick={
               e =>
                 _removeScriptEventFunction(currentScript, name, dispatchFunc)
             }>
             {DomHelper.textEl("Remove")}
           </button>
         </div>;
       });
  };

  let sortScriptEventFunctionNodeIds = scriptEventFunctionNodeIds =>
    scriptEventFunctionNodeIds |> Js.Array.sortInPlace;

  let handleChangeScriptEventFunction =
      (
        script,
        sendFunc,
        currentScriptEventFunctionNodeId,
        targetScriptEventFunctionNodeId,
      ) => {
    WonderLog.Contract.requireCheck(
      () =>
        WonderLog.(
          Contract.(
            Operators.(
              test(
                Log.buildAssertMessage(
                  ~expect={j|currentScriptEventFunctionNodeId|j},
                  ~actual={j|not|j},
                ),
                () =>
                currentScriptEventFunctionNodeId |> assertExist
              )
            )
          )
        ),
      StateEditorService.getStateIsDebug(),
    );

    MainEditorScriptUtils.isNodeIdEqual(
      currentScriptEventFunctionNodeId,
      targetScriptEventFunctionNodeId,
    ) ?
      () :
      {
        _changeScriptEventFunction(
          script,
          currentScriptEventFunctionNodeId,
          targetScriptEventFunctionNodeId,
        )
        |> StateLogicService.getStateToGetData
        |> StateEngineService.setState;

        sendFunc(
          targetScriptEventFunctionNodeId,
          _getUnUsedScriptEventFunctionNodeIds(script)
          |> StateLogicService.getStateToGetData,
        );
      };
  };

  let addScriptEventFunction = (languageType, (state, send)) => {
    let editorState = StateEditorService.getState();
    let engineState = StateEngineService.unsafeGetState();

    let {currentScript} = state;

    let unUsedScriptEventFunctionNodes =
      _getUnUsedScriptEventFunctionNodes(
        currentScript,
        (editorState, engineState),
      );

    unUsedScriptEventFunctionNodes |> Js.Array.length > 0 ?
      {
        let unUsedScriptEventFunctionNodeIds =
          unUsedScriptEventFunctionNodes
          |> Js.Array.map(node => NodeAssetService.getNodeId(~node));

        let (
          lastScriptEventFunctionNodeIdForAdd,
          unUsedScriptEventFunctionNodeIds,
        ) =
          unUsedScriptEventFunctionNodeIds |> ArrayService.removeFirst;

        let (name, attribute) =
          ScriptEventFunctionNodeAssetEditorService.getNameAndData(
            lastScriptEventFunctionNodeIdForAdd,
            editorState,
          );

        let engineState =
          ScriptEngineService.addScriptEventFunctionData(
            currentScript,
            name,
            attribute,
            engineState,
          );

        engineState |> StateEngineService.setState |> ignore;

        send(
          ShowScriptEventFunctionGroupForAdd(
            lastScriptEventFunctionNodeIdForAdd,
            unUsedScriptEventFunctionNodeIds,
          ),
        );
      } :
      ConsoleUtils.warn(
        LanguageUtils.getMessageLanguageDataByType(
          "need-add-scriptEventFunction",
          languageType,
        ),
      )
      |> StateLogicService.getEditorState;
  };

  let getAllScriptEventFunctions =
      (currentScriptEventFunctionNodeId, unUsedScriptEventFunctionNodeIds) =>
    ArrayService.fastConcat(
      [|currentScriptEventFunctionNodeId |> OptionService.unsafeGet|],
      unUsedScriptEventFunctionNodeIds,
    )
    |> sortScriptEventFunctionNodeIds;

  let isScriptEventFunction =
      (currentScriptEventFunctionNodeId, scriptEventFunctionNodeId) =>
    MainEditorScriptUtils.isNodeIdEqual(
      currentScriptEventFunctionNodeId,
      scriptEventFunctionNodeId,
    );

  let getSelectScriptEventFunctionGroupWidgetText = scriptEventFunctionNodeId =>
    OperateTreeAssetLogicService.unsafeGetNodeNameById(
      scriptEventFunctionNodeId,
    )
    |> StateLogicService.getStateToGetData;
};

let component = ReasonReact.reducerComponent("MainEditorScriptEventFunction");

let reducer = (action, state) =>
  switch (action) {
  | ChangeScriptEventFunctionForAdd(
      targetScriptEventFunctionNodeId,
      unUsedScriptEventFunctionNodeIds,
    ) =>
    ReasonReact.Update({
      ...state,
      lastScriptEventFunctionNodeIdForAdd:
        Some(targetScriptEventFunctionNodeId),
      unUsedScriptEventFunctionNodeIds,
    })
  | ChangeScriptEventFunctionForChange(
      targetScriptEventFunctionNodeId,
      unUsedScriptEventFunctionNodeIds,
    ) =>
    ReasonReact.Update({
      ...state,
      lastScriptEventFunctionNodeIdForChange:
        Some(targetScriptEventFunctionNodeId),
      unUsedScriptEventFunctionNodeIds,
    })
  | ShowScriptEventFunctionGroupForAdd(
      lastScriptEventFunctionNodeIdForAdd,
      unUsedScriptEventFunctionNodeIds,
    ) =>
    ReasonReact.Update({
      ...state,
      isShowScriptEventFunctionGroupForAdd: true,
      lastScriptEventFunctionNodeIdForAdd:
        Some(lastScriptEventFunctionNodeIdForAdd),
      unUsedScriptEventFunctionNodeIds,
    })
  | ShowScriptEventFunctionGroupForChange(
      lastScriptEventFunctionNodeIdForChange,
      unUsedScriptEventFunctionNodeIds,
    ) =>
    ReasonReact.Update({
      ...state,
      isShowScriptEventFunctionGroupForChange: true,
      lastScriptEventFunctionNodeIdForChange:
        Some(lastScriptEventFunctionNodeIdForChange),
      unUsedScriptEventFunctionNodeIds,
    })
  | HideScriptEventFunctionGroupForAdd =>
    ReasonReact.Update({
      ...state,
      isShowScriptEventFunctionGroupForAdd: false,
      lastScriptEventFunctionNodeIdForAdd: None,
    })
  | HideScriptEventFunctionGroupForChange =>
    ReasonReact.Update({
      ...state,
      isShowScriptEventFunctionGroupForChange: false,
      lastScriptEventFunctionNodeIdForChange: None,
    })
  };

let render =
    (
      (uiState, dispatchFunc),
      ({state, send}: ReasonReact.self('a, 'b, 'c)) as self,
    ) => {
  let languageType =
    LanguageEditorService.unsafeGetType |> StateLogicService.getEditorState;

  <article key="MainEditorScript" className="wonder-inspector-script">
    {
      state.isShowScriptEventFunctionGroupForAdd ?
        <SelectAssetGroupWidget
          headerText="Add Script Event Function"
          sendFunc=send
          clickHideGroupButtonFunc={
            send => send(HideScriptEventFunctionGroupForAdd)
          }
          getAllAssetsFunc={
            () =>
              Method.getAllScriptEventFunctions(
                state.lastScriptEventFunctionNodeIdForAdd,
                state.unUsedScriptEventFunctionNodeIds,
              )
          }
          isAssetFunc={
            scriptEventFunctionNodeId => {
              let currentScriptEventFunctionNodeId =
                state.lastScriptEventFunctionNodeIdForAdd;

              Method.isScriptEventFunction(
                currentScriptEventFunctionNodeId,
                scriptEventFunctionNodeId,
              );
            }
          }
          changeAssetFunc={
            (scriptEventFunctionNodeId, send) => {
              let currentScriptEventFunctionNodeId =
                state.lastScriptEventFunctionNodeIdForAdd;

              Method.handleChangeScriptEventFunction(
                state.currentScript,
                (
                  targetScriptEventFunctionNodeId,
                  unUsedScriptEventFunctionNodeIds,
                ) =>
                  send(
                    ChangeScriptEventFunctionForAdd(
                      targetScriptEventFunctionNodeId,
                      unUsedScriptEventFunctionNodeIds,
                    ),
                  ),
                currentScriptEventFunctionNodeId,
                scriptEventFunctionNodeId,
              );
            }
          }
          getTextFunc=Method.getSelectScriptEventFunctionGroupWidgetText
        /> :
        ReasonReact.null
    }
    {
      state.isShowScriptEventFunctionGroupForChange ?
        <SelectAssetGroupWidget
          headerText="Change Script Event Function"
          sendFunc=send
          clickHideGroupButtonFunc={
            send => send(HideScriptEventFunctionGroupForChange)
          }
          getAllAssetsFunc={
            () =>
              Method.getAllScriptEventFunctions(
                state.lastScriptEventFunctionNodeIdForChange,
                state.unUsedScriptEventFunctionNodeIds,
              )
          }
          isAssetFunc={
            scriptEventFunctionNodeId => {
              let currentScriptEventFunctionNodeId =
                state.lastScriptEventFunctionNodeIdForChange;

              Method.isScriptEventFunction(
                currentScriptEventFunctionNodeId,
                scriptEventFunctionNodeId,
              );
            }
          }
          changeAssetFunc={
            (scriptEventFunctionNodeId, send) => {
              let currentScriptEventFunctionNodeId =
                state.lastScriptEventFunctionNodeIdForChange;

              Method.handleChangeScriptEventFunction(
                state.currentScript,
                (
                  targetScriptEventFunctionNodeId,
                  unUsedScriptEventFunctionNodeIds,
                ) =>
                  send(
                    ChangeScriptEventFunctionForChange(
                      targetScriptEventFunctionNodeId,
                      unUsedScriptEventFunctionNodeIds,
                    ),
                  ),
                currentScriptEventFunctionNodeId,
                scriptEventFunctionNodeId,
              );
            }
          }
          getTextFunc=Method.getSelectScriptEventFunctionGroupWidgetText
        /> :
        ReasonReact.null
    }
    {
      ReasonReact.array(
        Method.renderScriptAllEventFunctions(
          languageType,
          dispatchFunc,
          self,
        ),
      )
    }
    <button
      className="addable-btn"
      onClick={
        _e => Method.addScriptEventFunction(languageType, (state, send))
      }>
      {
        DomHelper.textEl(
          LanguageUtils.getInspectorLanguageDataByType(
            "script-add-scriptEventFunction",
            languageType,
          ),
        )
      }
    </button>
  </article>;
};

let make = (~uiState, ~dispatchFunc, ~script, _children) => {
  ...component,
  initialState: () => {
    currentScript: script,
    isShowScriptEventFunctionGroupForAdd: false,
    isShowScriptEventFunctionGroupForChange: false,
    lastScriptEventFunctionNodeIdForAdd: None,
    lastScriptEventFunctionNodeIdForChange: None,
    unUsedScriptEventFunctionNodeIds:
      WonderCommonlib.ArrayService.createEmpty(),
  },
  reducer,
  render: self => render((uiState, dispatchFunc), self),
};