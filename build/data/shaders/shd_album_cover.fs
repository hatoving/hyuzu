#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform float corner_scale = 0.2;

// Output fragment color
out vec4 finalColor;

bool isOut(float x, float y) {
    return pow(x, 2.0) + pow(y, 2.0) > pow(corner_scale * 0.5, 2.0);
}

void main() {
    float s = corner_scale * 0.5;

    if(
        (fragTexCoord.x < s && fragTexCoord.y < s && isOut(fragTexCoord.x - s, fragTexCoord.y - s)) ||
        (fragTexCoord.x < s && fragTexCoord.y > 1.0 - s && isOut(fragTexCoord.x - s, fragTexCoord.y - 1.0 + s)) ||
        (fragTexCoord.x > 1.0 - s && fragTexCoord.y < s && isOut(fragTexCoord.x - 1.0 + s, fragTexCoord.y - s)) ||
        (fragTexCoord.x > 1.0 - s && fragTexCoord.y > 1.0 - s && isOut(fragTexCoord.x - 1.0 + s, fragTexCoord.y - 1.0 + s))
    ) {
        finalColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else {
        finalColor = texture(texture0, fragTexCoord) * fragColor;
    }
}
