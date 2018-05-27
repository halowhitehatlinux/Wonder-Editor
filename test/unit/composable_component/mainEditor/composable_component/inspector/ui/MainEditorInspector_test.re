/* open Wonder_jest;

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
                 MainEditorAssetTool.initAssetTree(MainEditorAssetTool.buildTwoLayerAssetTree)
              )
          );
          test(
            "if not set currentSelectSource, show nothing",
            () =>
              BuildComponentTool.buildInspectorComponent(
                TestTool.buildEmptyAppState(),
                InspectorTool.buildFakeAllShowComponentConfig()
              )
              |> ReactTestTool.createSnapshotAndMatch
          );
          describe(
            "else set currentSelectSource is SceneTree",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    MainEditorSceneTool.setFirstBoxTobeCurrentSceneTreeNode
                  );
                  CurrentSelectSourceEditorService.setCurrentSelectSource(EditorType.SceneTree)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentSceneTreeNode component",
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
            "else set currentSelectSource is AssetFile",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    MainEditorAssetTool.initAssetTree(MainEditorAssetTool.buildTwoLayerAssetTree)
                  );
                  CurrentSelectSourceEditorService.setCurrentSelectSource(EditorType.AssetFile)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentNodeId component",
                () => {
                  MainEditorAssetTool.setImgFileToBeCurrentNodeId();
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
            "else set currentSelectSource is AssetTree",
            () => {
              beforeEach(
                () => {
                  MainEditorSceneTool.createDefaultScene(
                    sandbox,
                    MainEditorAssetTool.initAssetTree(MainEditorAssetTool.buildTwoLayerAssetTree)
                  );
                  CurrentSelectSourceEditorService.setCurrentSelectSource(EditorType.AssetTree)
                  |> StateLogicService.getAndSetEditorState
                }
              );
              test(
                "show currentAssetChildrenNodeParent component",
                () => {
                  MainEditorAssetTool.setFolder1ToBeCurrentAssetChildrenNodeParent();
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
  ); */