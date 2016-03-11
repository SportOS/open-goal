// Fill out your copyright notice in the Description page of Project Settings.

#include "OSF.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, OSF, "OSF" );

void _softassert(bool expr) {
    if (!expr) {
        // breakpoint here
        UE_LOG(LogTemp, Warning, TEXT("Soft assertion failed"));
    }
}