!isEmpty(SGEN_DESKTOP_DEPLOY_PRI_INLUDED):error("Desktop deployment already included")
SGEN_DESKTOP_DEPLOY_PRI_INLUDED = 1

BUILD_DIR = $$clean_path($$OUT_PWD)

DEPLOY_MODE = release
CONFIG(debug, debug|release):DEPLOY_MODE = debug

deployfiles.target = extra

win32 {
    deployfiles.commands = \"$$PWD/scripts/win_deploy.bat\" \"%QTDIR%\" $$PWD $${BUILD_DIR}/$${DEPLOY_MODE} \"$${DEPLOY_MODE}\"
} else:win64 {
    deployfiles.commands = \"$$PWD/scripts/win_deploy.bat\" \"%QTDIR%\" $$PWD $${BUILD_DIR}/$${DEPLOY_MODE} \"$${DEPLOY_MODE}\"
} else:unix {
    deployfiles.commands = \"$$PWD/scripts/lin_deploy.sh\" \"$$PWD\" \"$$OUT_PWD\" \"$${DEPLOY_MODE}\"
}

win32 {
    deployfiles.commands ~= s,/,\\\\,g
}

QMAKE_EXTRA_TARGETS += deployfiles
PRE_TARGETDEPS = extra
