**_20/03/2022_**

- Add activeModalities as a bool vector in ExqController and pass it by reference to ExqDataHandler and ExqFunctions.
- ExqPyInterface needs a function to update activeModalities. Argument would be a list of bool. Return with an error message if all modalities are set as False.
- When ExqDataHandler is getting data, check if modality is active beforehand. Similarly when adding distance for modality in ExqWorker check with the ExqDataHandler if modality is active.
- Further improvement to this would be to have a merge function that takes two models and modality preference. After training and selecting items from segments the final lists are combined and sorted based on aggScore of the items. Initial idea, changes with regards to weighing a modality higher or lower could also be added.


**_Prior to 20/03/2022_**

Dependencies
=============

ExqClassifier


ExqDataHandler  ->  ExqDataHandlerH5, ExqDataHandlerECP


ExqFunctions    ->  ExqFunctionsR64


ExqWorker


ExqController   --  ExqFunctions per Modality
                --  ExqDataHandler
                --  ExqClassifier per Modality
                --  ExqWorker


ExqPyInterface  --  ExqController



ExqMain         --  ExqController (no priority)



Program Flow
=============

Initialize:

    1. ExqPyInterface gets arguments
    2. Initialize ExqFunctions, ExqDataHandler(load data) and ExqClassifier
    3. Initialize ExqController with other Exq classes
    4. Initialize ExqWorker threadpool (if necessary)
    5. ExqController sends success/failure message to ExqPyInterface that returns it


Train:

    1. ExqPyInterface gets arguments and calls ExqController->train
    2. ExqController utilizes ExqFunctions to extract feature data and pass this to ExqClassifier to train the model
    3. ExqController uses the Classifier to select clusters (eCP only) (use ExqWorker?)
    4. ExqController sends success/failure message to ExqPyInterface that returns it


Suggest:

    1. ExqPyInterface gets arguments and calls ExqController->suggest
    2. ExqController splits the work among s segments and for each segment runs ExqFunctions->suggest
    3. ExqController ranks the combined result set from the segments using ExqFunctions->rankItems
    4. ExqController sends k or less suggestions with a success/failure message to ExqPyInterface that returns it


Terminate:

    1. ExqPyInterface saves the logs and deletes the ExqController and then exits
    
