# Репозиторий плагина для UE, который создаёт разметки kitty и json
**MakeMarkup**
BluePrint, который принимает ссылку на камеру(**_USceneCaptureComponent2D\* Camera_**), массив объектов(*AActor*) на сцене(**_TArray<AActor\*> actors_**), коллекцию объектов с их границами (**_TMap<UObject\*, FVector> map_**), логическое значение(**_bool RelativeCoordinates_**) для отображения координат относительно размеров камеры. Возвращает массив разметок(**_TArray<FMarkupStruct>_**).
![image info](./MakeMarkup.png)

**GenerateJsonFile**
BluePrint, который принимает массив разметок(**_TArray<FMarkupStruct> array_**), имя файла(**_FString nameFile_**), логическое значение(**_bool center_**), для возвращения координат либо центра , либо левого верхнего угла объекта. Создаёт файл, в который помещает разметки в формате json.
![image info](./GenerateJsonFile.png)

**GenerateKittyFile**
BluePrint, который принимает массив разметок(**_TArray<FMarkupStruct> array_**), имя файла(**_FString nameFile_**), индекс траектории(**_int32 trajectory_index_**), код погоды(**_int32 WeatherCode_**), старт траектории(**_bool TrajectoryStart_**), логическое значение(**_bool center_**), для возвращения координат либо центра , либо левого верхнего угла объекта. Создаёт файл, в который помещает разметки в формате kitty.
![image info](./GenerateKittyFile.png)

Также есть BluePrint **_GenerateJsonStrings_**, который возвращает json разметку массивом строк, и **_GenerateKittyStrings_**, который возвращает kitty разметку массивом строк.
