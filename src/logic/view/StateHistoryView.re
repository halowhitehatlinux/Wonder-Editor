let storeHistoryState = (uiState, editorState, engineState, historyState) =>
  historyState
  |> UIStateHistory.storeUIState(uiState)
  |> EditorStateView.storeEditorState(editorState)
  |> EngineStateView.storeEngineState(engineState);

let _operateEngineStateHistory = (operateHistoryFunc) =>
  EngineStateView.getEngineState()
  |> operateHistoryFunc
  |> EngineStateView.setEngineState
  |> ignore;

let _operateEditorStateHistory = (operateHistoryFunc) =>
  EditorStateView.getEditorState()
  |> operateHistoryFunc
  |> EditorStateView.setEditorState
  |> ignore;

let undoHistoryState = (store, dispatch) => {
  dispatch(AppStore.ReplaceState(UIStateHistory.undo(AllStateData.getHistoryState(), store)));
  _operateEngineStateHistory(EngineStateView.undo(AllStateData.getHistoryState()));
  _operateEditorStateHistory(EditorStateView.undo(AllStateData.getHistoryState()))
};

let redoHistoryState = (store, dispatch) => {
  dispatch(AppStore.ReplaceState(UIStateHistory.redo(AllStateData.getHistoryState(), store)));
  _operateEngineStateHistory(EngineStateView.redo(AllStateData.getHistoryState()));
  _operateEditorStateHistory(EditorStateView.redo(AllStateData.getHistoryState()))
};