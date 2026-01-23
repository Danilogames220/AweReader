include("/home/assuero/Documentos/computaria/cpp/pdf-reader/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/reader-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/assuero/Documentos/computaria/cpp/pdf-reader/reader"
    GENERATE_QT_CONF
)
