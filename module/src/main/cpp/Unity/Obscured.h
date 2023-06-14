#pragma once

int GetObscuredIntValue(uintptr_t location){
    int cryptoKey = *(int *) location;
    int obfuscatedValue = *(int *) (location + 0x8);
    return obfuscatedValue ^ cryptoKey;
}

void SetObscuredIntValue(uintptr_t location, int value){
	int cryptoKey = *(int *) location;
	
	*(int *) (location + 0x8) = value ^ cryptoKey;
}

float GetObscuredFloatValue(uintptr_t location){
	int cryptoKey = *(int *) location;
	int obfuscatedValue = *(int *) (location + 0x8);
	
	union intfloat {
		int i;
		float f;
	};
	
	/* use this intfloat to set the integer representation of our parameter value, which will also set the float value */
	intfloat IF;
	IF.i = obfuscatedValue ^ cryptoKey;
	
	return IF.f;
}

void SetObscuredFloatValue(uintptr_t location, float value){
	int cryptoKey = *(int *) location;
	
	union intfloat {
		int i;
		float f;
	};
	
	/* use this intfloat to get the integer representation of our parameter value */
	intfloat IF;
	IF.f = value;
	
	/* use this intfloat to generate our hacked ObscuredFloat */
	intfloat IF2;
	IF2.i = IF.i ^ cryptoKey;
	
	*(float *) (location + 0x8) = IF2.f;
}
