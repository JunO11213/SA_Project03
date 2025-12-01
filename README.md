프로젝트 속성 - C/C++ - 전처리기 - _DEBUG;_CONSOLE;USE_STUB;%(PreprocessorDefinitions)로 변경

gcc -D USE_STUB -o test_runner.exe test_main.c test_unit.c test_integration.c test_system.c motor_stub.c unity.c 1.c
