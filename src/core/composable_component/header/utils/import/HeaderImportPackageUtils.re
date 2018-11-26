open Js.Promise;

open Js.Typed_array;

let _disposeAssets = () =>
  StateLogicService.getAndSetStateToGetData(
    RemoveWholeAssetTreeAssetLogicService.deepDisposeAssetTreeRoot,
  );

let _readHeader = dataView => {
  let (version, byteOffset) = DataViewUtils.getUint32_1(. 0, dataView);

  let (sceneWDBByteLength, byteOffset) =
    DataViewUtils.getUint32_1(. byteOffset, dataView);

  let (asbByteLength, byteOffset) =
    DataViewUtils.getUint32_1(. byteOffset, dataView);

  (byteOffset, version, sceneWDBByteLength, asbByteLength);
};

let _readWPK = (wpk, dataView) => {
  let (byteOffset, version, sceneWDBByteLength, asbByteLength) =
    _readHeader(dataView);

  (
    version,
    wpk
    |> ArrayBuffer.slice(
         ~start=byteOffset,
         ~end_=byteOffset + sceneWDBByteLength,
       ),
    wpk
    |> ArrayBuffer.sliceFrom(
         byteOffset + (sceneWDBByteLength |> BufferUtils.alignedLength),
       ),
    dataView,
  );
};

let _initAssetTreeRoot = () => {
  let editorState = StateEditorService.getState();
  let engineState = StateEngineService.unsafeGetState();

  let (assetTree, editorState) =
    editorState |> AssetTreeUtils.initRootAssetTree(_, engineState);

  editorState
  |> TreeRootAssetEditorService.setAssetTreeRoot(assetTree)
  |> StateEditorService.setState
  |> ignore;

  engineState |> StateEngineService.setState |> ignore;

  ();
};

let _reInitDefaultMaterials = (editorState, engineState) => {
  let engineState =
    [|
      MaterialDataAssetEditorService.unsafeGetDefaultBasicMaterial(
        editorState,
      ),
    |]
    |> BasicMaterialEngineService.reInitMaterials(_, engineState);

  [|
    MaterialDataAssetEditorService.unsafeGetDefaultLightMaterial(editorState),
  |]
  |> LightMaterialEngineService.reInitMaterials(_, engineState);
};

let _import = result => {
  _disposeAssets();

  StateEngineService.unsafeGetState()
  |> JobEngineService.execDisposeJob
  |> ReallocateCPUMemoryJobUtils.reallocate(0.1)
  |> StateEngineService.setState
  |> ignore;

  _initAssetTreeRoot();

  let wpk = result |> FileReader.convertResultToArrayBuffer;

  let dataView = DataViewUtils.create(wpk);

  let (version, sceneWDB, asb, dataView) = _readWPK(wpk, dataView);

  let materialMapTupleRef =
    ref((
      WonderCommonlib.SparseMapService.createEmpty(),
      WonderCommonlib.SparseMapService.createEmpty(),
    ));

  let imageUint8ArrayDataMapRef =
    ref(WonderCommonlib.SparseMapService.createEmpty());

  let allWDBGameObjectsArrRef = ref([||]);

  let wdbAssetGameObjectGeometryAssetArrRef = ref([||]);

  let engineState = StateEngineService.unsafeGetState();

  HeaderImportASBUtils.importASB(asb)
  |> WonderBsMost.Most.map(
       (((allWDBGameObjectsArr, imageUint8ArrayDataMap), materialMapTuple)) => {
       ImportPackageRelateGameObjectAndAssetUtils.relateWDBAssetGameObjectsAndAssets(
         allWDBGameObjectsArr,
         materialMapTuple,
         imageUint8ArrayDataMap,
       );

       allWDBGameObjectsArrRef := allWDBGameObjectsArr;
       materialMapTupleRef := materialMapTuple;
       wdbAssetGameObjectGeometryAssetArrRef :=
         GeometryAssetLogicService.getGeometryAssetsFromWDBGameObjects(
           allWDBGameObjectsArr,
         )
         |> StateLogicService.getStateToGetData;
       imageUint8ArrayDataMapRef := imageUint8ArrayDataMap;

       ();
     })
  |> WonderBsMost.Most.concat(
       MostUtils.callStreamFunc(() =>
         SceneWDBUtils.importSceneWDB(sceneWDB)
         |> WonderBsMost.Most.map(
              ((sceneGameObject, imageUint8ArrayDataMap)) => {
              let engineState = StateEngineService.unsafeGetState();

              ImportPackageRelateGameObjectAndAssetUtils.relateSceneWDBGameObjectsAndAssets(
                GameObjectEngineService.getAllGameObjects(
                  sceneGameObject,
                  engineState,
                ),
                /* TODO use asset->imageUint8ArrayDataMap? */
                SparseMapService.mergeSparseMaps([|
                  imageUint8ArrayDataMapRef^,
                  imageUint8ArrayDataMap,
                |]),
                materialMapTupleRef^,
                wdbAssetGameObjectGeometryAssetArrRef^,
              );

              ();
            })
       ),
     )
  |> WonderBsMost.Most.concat(
       MostUtils.callFunc(() => {
         WonderLog.Contract.requireCheck(
           () =>
             WonderLog.(
               Contract.(
                 test(
                   Log.buildAssertMessage(
                     ~expect=
                       {j|all scene gameObjects->textures should be texture assets|j},
                     ~actual={j|not|j},
                   ),
                   () => {
                     let sceneTextures =
                       GameObjectEngineService.getAllLightMaterials(
                         SceneEngineService.getSceneGameObject(
                           StateEngineService.unsafeGetState(),
                         )
                         |> GameObjectEngineService.getAllGameObjects(
                              _,
                              StateEngineService.unsafeGetState(),
                            ),
                         StateEngineService.unsafeGetState(),
                       )
                       |> Js.Array.map(material =>
                            LightMaterialEngineService.getLightMaterialDiffuseMap(
                              material,
                              StateEngineService.unsafeGetState(),
                            )
                          )
                       |> Js.Array.filter(diffuseMap =>
                            diffuseMap |> Js.Option.isSome
                          )
                       |> Js.Array.map(diffuseMap =>
                            diffuseMap |> OptionService.unsafeGet
                          );

                     sceneTextures |> Js.Array.sortInPlace;

                     let textureAssets =
                       TextureNodeMapAssetEditorService.getTextureComponents(
                         StateEditorService.getState(),
                       );

                     textureAssets |> Js.Array.sortInPlace;

                     ArrayService.isInclude(textureAssets, sceneTextures)
                     |> assertTrue;
                   },
                 )
               )
             ),
           StateEditorService.getStateIsDebug(),
         );

         let editorState = StateEditorService.getState();
         let engineState = StateEngineService.unsafeGetState();

         let engineState = engineState |> ShaderEngineService.clearShaderCache;

         let engineState = _reInitDefaultMaterials(editorState, engineState);

         let engineState =
           ArrayService.fastConcat(
             allWDBGameObjectsArrRef^,
             GameObjectEngineService.getAllGameObjects(
               SceneEngineService.getSceneGameObject(engineState),
               engineState,
             ),
           )
           |> WonderCommonlib.ArrayService.reduceOneParam(
                (. engineState, gameObject) =>
                  GameObjectEngineService.initGameObject(
                    gameObject,
                    engineState,
                  ),
                engineState,
              );

         StateLogicService.refreshEngineState(engineState);
       }),
     );
};

let importPackage = (dispatchFunc, event) => {
  let editorState = StateEditorService.getState();

  StateEditorService.getIsRun() ?
    {
      ConsoleUtils.warn(
        "should import package when stop, but now is run!",
        editorState,
      );

      Js.Promise.make((~resolve, ~reject) =>
        resolve(.
          dispatchFunc(
            AppStore.UpdateAction(Update([|UpdateStore.NoUpdate|])),
          ),
        )
      );
    } :
    {
      let e = ReactEventType.convertReactFormEventToJsEvent(event);
      DomHelper.preventDefault(e);

      switch (e##target##files |> Js.Dict.values |> ArrayService.getFirst) {
      | None =>
        Js.Promise.make((~resolve, ~reject) =>
          resolve(.
            dispatchFunc(
              AppStore.UpdateAction(Update([|UpdateStore.NoUpdate|])),
            ),
          )
        )
      | Some(file) =>
        let fileInfo: FileType.fileInfoType =
          file |> FileReader.convertFileJsObjectToFileInfoRecord;

        WonderBsMost.Most.fromPromise(
          Js.Promise.make((~resolve, ~reject) => {
            let reader = FileReader.createFileReader();

            FileReader.onload(reader, result =>
              resolve(.
                {
                  name: fileInfo.name,
                  type_: LoadAssetUtils.getUploadPackageType(fileInfo.name),
                  result,
                }: AssetNodeType.nodeResultType,
              )
            );

            LoadAssetUtils.readPakckageByTypeSync(reader, fileInfo);
          }),
        )
        |> WonderBsMost.Most.flatMap(
             (fileResult: AssetNodeType.nodeResultType) =>
             _import(fileResult.result)
           )
        |> WonderBsMost.Most.drain
        |> then_(_ => {
             StackHistoryService.clearAllStack(
               AllStateData.getHistoryState(),
             );

             dispatchFunc(
               AppStore.SceneTreeAction(
                 SetSceneGraph(
                   Some(
                     SceneGraphUtils.getSceneGraphDataFromEngine
                     |> StateLogicService.getStateToGetData,
                   ),
                 ),
               ),
             );
             dispatchFunc(
               AppStore.UpdateAction(Update([|UpdateStore.All|])),
             )
             |> resolve;
           });
      };
    };
};