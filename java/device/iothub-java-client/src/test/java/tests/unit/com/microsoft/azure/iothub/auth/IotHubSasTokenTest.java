// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.auth;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;

import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.auth.Signature;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import org.junit.Test;

/** Unit tests for IotHubSasToken. */
public class IotHubSasTokenTest
{
    @Mocked Signature mockSig;

    // Tests_SRS_IOTHUBSASTOKEN_11_001: [The SAS token shall have the format "SharedAccessSignature sig=<signature >&se=<expiryTime>&skn=&sr=<resourceURI>". The params can be in any order.]
    @Test
    public void sasTokenHasCorrectFormat()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";
        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token = new IotHubSasToken(resourceUri, deviceId, deviceKey, expiryTime);
        token.appendSknValue = false;
        String tokenStr = token.toString();

        // assert that sig, se, skn, and sr exist in the token in any order.
        assertThat(tokenStr.matches("SharedAccessSignature "
                + "((sig|se|sr)=[^&=]+|skn=)&((sig|se|sr)=[^&=]+|skn=)"
                + "&((sig|se|sr)=[^&=]+|skn=)&((sig|se|sr)=[^&=]+|skn=)"), is(true));

        assertThat(tokenStr.indexOf("sig="), is(not(-1)));
        assertThat(tokenStr.indexOf("se="), is(not(-1)));
        assertThat(tokenStr.indexOf("skn="), is(not(-1)));
        assertThat(tokenStr.indexOf("sr="), is(not(-1)));
    }

    // Tests_SRS_IOTHUBSASTOKEN_11_002: [The expiry time shall be the given expiry time, where it is a UNIX timestamp and indicates the time after which the token becomes invalid.]
    @Test
    public void expiryTimeSetCorrectly()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";
        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token =
                new IotHubSasToken(resourceUri, deviceId, deviceKey,
                        expiryTime);
        String tokenStr = token.toString();
        // extract the value assigned to se.
        int expiryTimeKeyIdx = tokenStr.indexOf("se=");
        int expiryTimeStartIdx = expiryTimeKeyIdx + 3;
        int expiryTimeEndIdx =
                tokenStr.indexOf("&", expiryTimeStartIdx);
        if (expiryTimeEndIdx == -1)
        {
            expiryTimeEndIdx = tokenStr.length();
        }
        String testExpiryTimeStr =
                tokenStr.substring(expiryTimeStartIdx,
                        expiryTimeEndIdx);

        String expectedExpiryTimeStr = Long.toString(expiryTime);
        assertThat(testExpiryTimeStr, is(expectedExpiryTimeStr));
    }

    @Test
    public void keyNameEqualToDeviceId()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";
        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token =
                new IotHubSasToken(resourceUri, deviceId, deviceKey,
                        expiryTime);
        token.appendSknValue = false;
        String tokenStr = token.toString();
        // extract the value assigned to skn.
        int keyNameKeyIdx = tokenStr.indexOf("skn=");
        int keyNameStartIdx = keyNameKeyIdx + 4;
        int keyNameEndIdx = tokenStr.indexOf("&", keyNameStartIdx);
        if (keyNameEndIdx == -1)
        {
            keyNameEndIdx = tokenStr.length();
        }
        String testKeyName =
                tokenStr.substring(keyNameStartIdx, keyNameEndIdx);

        String expectedKeyName = "";
        assertThat(testKeyName, is(expectedKeyName));
    }

    @Test
    public void keyNameIsSuppressedForIoTHub()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";

        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token = new IotHubSasToken(resourceUri, deviceId, deviceKey, expiryTime);
        token.appendSknValue = true;

        String tokenStr = token.toString();
        // extract the value assigned to skn.
        int keyNameKeyIdx = tokenStr.indexOf("skn=");
        int keyNameStartIdx = keyNameKeyIdx + 4;
        int keyNameEndIdx = tokenStr.indexOf("&", keyNameStartIdx);
        if (keyNameEndIdx == -1)
        {
            keyNameEndIdx = tokenStr.length();
        }
        String testKeyName = tokenStr.substring(keyNameStartIdx, keyNameEndIdx);

        String expectedKeyName = deviceId;
        assertThat(testKeyName, is(expectedKeyName));
    }

    // Tests_SRS_IOTHUBSASTOKEN_11_004: [The resource URI shall be the given resource URI.]
    @Test
    public void resourceUriSetCorrectly()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";
        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token =
                new IotHubSasToken(resourceUri, deviceId, deviceKey,
                        expiryTime);
        String tokenStr = token.toString();
        // extract value assigned to sr.
        int resourceUriKeyIdx = tokenStr.indexOf("sr=");
        int resourceUriStartIdx = resourceUriKeyIdx + 3;
        int resourceUriEndIdx =
                tokenStr.indexOf("&", resourceUriStartIdx);
        if (resourceUriEndIdx == -1)
        {
            resourceUriEndIdx = tokenStr.length();
        }
        String testResourceUri =
                tokenStr.substring(resourceUriStartIdx,
                        resourceUriEndIdx);

        String expectedResourceUri = resourceUri;
        assertThat(testResourceUri, is(expectedResourceUri));
    }

    // Tests_SRS_IOTHUBSASTOKEN_11_005: [The signature shall be correctly computed and set.]
    // Tests_SRS_IOTHUBSASTOKEN_11_006: [The function shall return the string representation of the SAS token.]
    @Test
    public void signatureSetCorrectly()
    {
        final String resourceUri = "sample-resource-uri";
        final String deviceId = "sample-device-id";
        final String deviceKey = "sample-device-key";
        final long expiryTime = 100;
        final String signature = "sample-sig";
        new NonStrictExpectations()
        {
            {
                mockSig.toString();
                result = signature;
            }
        };

        IotHubSasToken token =
                new IotHubSasToken(resourceUri, deviceId, deviceKey,
                        expiryTime);
        String tokenStr = token.toString();
        // extract the value assigned to sig.
        int signatureKeyIdx = tokenStr.indexOf("sig=");
        int signatureStartIdx = signatureKeyIdx + 4;
        int signatureEndIdx =
                tokenStr.indexOf("&", signatureStartIdx);
        if (signatureEndIdx == -1)
        {
            signatureEndIdx = tokenStr.length();
        }
        String testSignature = tokenStr.substring(signatureStartIdx,
                signatureEndIdx);

        final String expectedSignature = signature;
        assertThat(testSignature, is(expectedSignature));
    }
}
