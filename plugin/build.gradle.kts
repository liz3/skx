import org.jetbrains.kotlin.backend.common.push
import com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar


plugins {
    java
    kotlin("jvm") version "1.3.72"
    id("com.github.johnrengelman.shadow") version "5.1.0"

}

group = "bet.liz3"
version = "0.0.1"

repositories {
    mavenCentral()
    maven(url = "https://hub.spigotmc.org/nexus/content/repositories/snapshots/")
    maven(url = "https://oss.sonatype.org/content/repositories/snapshots")
}

dependencies {
    implementation(kotlin("stdlib-jdk8"))
    compileOnly("org.spigotmc:spigot-api:1.12.2-R0.1-SNAPSHOT")

    testCompile("junit", "junit", "4.12")
}

configure<JavaPluginConvention> {
    sourceCompatibility = JavaVersion.VERSION_11
}

tasks {
    compileKotlin {
        kotlinOptions.jvmTarget = "1.8"
    }
    val copyResources by registering(Copy::class) {
        from("src/main/resources")
        into(buildDir.resolve("resources/main"))
      //  dependsOn(processResources)
    }
    val build by existing {
        dependsOn(shadowJar)
    }

    val shadowJar = named<ShadowJar>("shadowJar") {
        dependsOn(copyResources)
        mergeServiceFiles()
        exclude("META-INF/*.DSA")
        exclude("META-INF/*.RSA")
        archiveFileName.set("skx-${version}.jar")
    }

    compileTestKotlin {
        kotlinOptions.jvmTarget = "1.8"
    }
    compileJava {
    options.compilerArgs.push("-h")
    options.compilerArgs.push("$buildDir")
    }
}