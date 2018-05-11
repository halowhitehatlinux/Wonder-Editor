open Wonder_jest;

open Expect;

open Expect.Operators;

open Sinon;

let _ =
  describe(
    "MainEditorInspector",
    () => {
      let sandbox = getSandboxDefaultVal();
      beforeEach(
        () => {
          sandbox := createSandbox();
          MainEditorSceneTool.initStateAndGl(~sandbox, ())
        }
      );
      afterEach(() => restoreSandbox(refJsObjToSandbox(sandbox^)));
      describe(
        "change source to show it's inspector",
        () => {
          beforeEach(
            () =>
              MainEditorSceneTool.createDefaultScene(
                sandbox,
                () => MainEditorInspectorTool.initInspector()
              )
          );
          test(
            "if not set currentSource, show nothing",
            () =>
              BuildComponentTool.buildInspectorComponent(
                TestTool.buildEmptyAppState(),
                InspectorTool.buildFakeAllShowComponentConfig()
              )
              |> ReactTestTool.createSnapshotAndMatch
          );
          describe(
            "else set currentSource is SceneTree",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    MainEditorSceneTool.setFirstBoxTobeCurrentGameObject
                  );
                  CurrentSourceEditorService.setCurrentSource(EditorType.SceneTree)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentGameObject component",
                () =>
                  BuildComponentTool.buildInspectorComponent(
                    TestTool.buildEmptyAppState(),
                    InspectorTool.buildFakeAllShowComponentConfig()
                  )
                  |> ReactTestTool.createSnapshotAndMatch
              )
            }
          );
          describe(
            "else set currentSource is AssetFile",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    () => MainEditorInspectorTool.initInspector()
                  );
                  CurrentSourceEditorService.setCurrentSource(EditorType.AssetFile)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentFile component",
                () => {
                  MainEditorInspectorTool.setImgFileToBeCurrentFile();
                  BuildComponentTool.buildInspectorComponent(
                    TestTool.buildEmptyAppState(),
                    InspectorTool.buildFakeAllShowComponentConfig()
                  )
                  |> ReactTestTool.createSnapshotAndMatch
                }
              )
            }
          );
          describe(
            "else set currentSource is AssetTree",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    () => MainEditorInspectorTool.initInspector()
                  );
                  CurrentSourceEditorService.setCurrentSource(EditorType.AssetTree)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentTreeNode component",
                () => {
                  MainEditorInspectorTool.setFolderToBeCurrentTreeNode();
                  BuildComponentTool.buildInspectorComponent(
                    TestTool.buildEmptyAppState(),
                    InspectorTool.buildFakeAllShowComponentConfig()
                  )
                  |> ReactTestTool.createSnapshotAndMatch
                }
              )
            }
          )
        }
      )
    }
  );