// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

// #define softassert(expr) { if (!(expr)) { _DebugBreakAndPromptForRemote(); } }
void _softassert(bool expr);
#define softassert(expr) _softassert(expr)
#define hope(expr) _softassert(expr)

#define simpleclamp(expr, min, max) ((expr < min) ? min : ((expr > max) ? max : expr))
